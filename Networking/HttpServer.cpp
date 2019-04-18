//
// Created by Kim on 15/07/2018.
//

#define USE_STANDALONE_ASIO 1
#define ASIO_STANDALONE 1
#define ASIO_NO_TYPEID 1
#define ASIO_NO_EXCEPTIONS 1
#define _WIN32_WINNT 0x0501

#define HEADER_SIZE_MAX 4096

#include <asio.hpp>
#include <unordered_map>
#include <chrono>
#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include <Core/Identification.h>
#include <Core/Algorithms.h>
#include <deque>
#include <Core/Std.h>

#include "HttpServer.h"
#include "HttpRequest.h"
#include "Server.h"
#include "TcpStream.h"
#include "NetworkingModule.h"

#ifndef WIN32
#define stricmp strcasecmp
#endif

struct HttpStreamPartialRequest {
    std::vector<char> headerData;
    std::vector<char> contentData;
    s64 contentLength;
    Entity request;
    std::vector<char> responseQueue;
};

static void RouteRequest(Entity server, Entity request, Entity response);

void ApplyResponseContentHeaders(Entity server, Entity request, Entity response) {
    auto contentLengthHeader = FindHeaderType("Content-Length");
    auto contentTypeHeader = FindHeaderType("Content-Type");
    auto keepAliveHeader = FindHeaderType("Keep-Alive");
    auto connectionHeader = FindHeaderType("Connection");
    auto serverHeader = FindHeaderType("Server");
    auto cacheControlHeader = FindHeaderType("Cache-Control");

    u64 contentLength = 0;

    auto requestData = GetHttpRequest(request);
    auto responseData = GetHttpResponse(response);
    auto contentStream = responseData.HttpResponseContentStream;

    Entity header = 0;

    if(HasComponent(contentStream, ComponentOf_Stream())) {
        auto fileType = GetStream(contentStream).StreamFileType;
        if(StreamOpen(contentStream, StreamMode_Read)) {
            StreamSeek(contentStream, StreamSeek_End);

            contentLength = StreamTell(contentStream);

            StreamClose(contentStream);
        }

        if(IsEntityValid(fileType)) {
            header = CreateEntity();
            SetHttpHeader(header, {contentTypeHeader, GetFileType(fileType).FileTypeMimeType});
            responseData.HttpResponseHeaders.Add(header);
        }
    }

    char headerValue[16];
    snprintf(headerValue, sizeof(headerValue), "%llu", contentLength);

    header = CreateEntity();
    SetHttpHeader(header, {contentLengthHeader, headerValue});
    responseData.HttpResponseHeaders.Add(header);

    header = CreateEntity();
    SetHttpHeader(header, {cacheControlHeader, "max-age=1"});
    responseData.HttpResponseHeaders.Add(header);

    for(auto requestHeader : requestData.HttpRequestHeaders) {

        // If we have keep-alive, apply this to response as well
        if(GetHttpHeader(requestHeader).HttpHeaderType == connectionHeader) {

            header = CreateEntity();
            SetHttpHeader(header, {connectionHeader, GetHttpHeader(requestHeader).HttpHeaderValue});
            responseData.HttpResponseHeaders.Add(header);

            char keepAliveHeaderValue[256];
            snprintf(keepAliveHeaderValue, sizeof(keepAliveHeaderValue), "timeout=%hu, max=%hu", GetHttpServer(server).HttpServerKeepAliveTimeout, GetHttpServer(server).HttpServerKeepAliveMaxConnections);

            header = CreateEntity();
            SetHttpHeader(header, {keepAliveHeader, keepAliveHeaderValue});
            responseData.HttpResponseHeaders.Add(header);
        }
    }

    header = CreateEntity();
    SetHttpHeader(header, {serverHeader, GetIdentification(server).Uuid});
    responseData.HttpResponseHeaders.Add(header);
    SetHttpResponse(response, responseData);
}

static void ParseQueryParams(Entity request, HttpRequest& data, StringRef queryParams) {
    auto paramsBuffer = (char*)alloca(strlen(queryParams) + 1);
    strcpy(paramsBuffer, queryParams);

    auto nextParam = paramsBuffer;
    while(nextParam && *nextParam) {
        auto param = nextParam;
        nextParam = strchr(nextParam, '&');
        if(nextParam) {
            *nextParam = '\0';
            nextParam++;
        }

        auto parameter = CreateEntity();
        HttpParameter parameterData = GetHttpParameter(parameter);

        auto value = strchr(param, '=');
        if(value) {
            *value = '\0';
            value++;

            parameterData.HttpParameterValue = value;
        } else {
            parameterData.HttpParameterValue = 0;
        }

        parameterData.HttpParameterName = param;

        SetHttpParameter(parameter, parameterData);
        data.HttpRequestParameters.Add(parameter);
    }
}

static void ParseHeader(Entity server, HttpStreamPartialRequest *partial, char *headerLine) {
    auto requestData = GetHttpRequest(partial->request);
    auto colonLocation = strstr(headerLine, ": ");
    if(colonLocation) {
        // This is a http header
        *colonLocation = '\0';
        auto identifier = headerLine;
        auto value = Intern(colonLocation + 2);

        auto headerType = FindHeaderType(identifier);
        if(!headerType) {
            Log(partial->request, LogSeverity_Warning, "Unrecognized header: '%s: %s'", identifier, value);
        } else {
            auto header = CreateEntity();
            SetHttpHeader(header, {headerType, value});

            requestData.HttpRequestHeaders.Add(header);

            if(stricmp(identifier, "content-length") == 0) {
                partial->contentLength = strtol(value, NULL, 10);
            }

            if(stricmp(identifier, "content-type") == 0) {
                auto charSet = strstr(value, ";");

                // Fake null terminator to leave out charset separator for comparing mime type
                if(charSet) {
                    *charSet = '\0';
                }

                FileType fileTypeData;
                for_entity_data(fileType, ComponentOf_FileType(), &fileTypeData) {
                    if(fileTypeData.FileTypeMimeType == value) {
                        char streamPath[PathMax];
                        snprintf(streamPath, PathMax, "memory://request%s", GetFileType(fileType).FileTypeExtension);

                        auto contentStream = GetHttpRequest(partial->request).HttpRequestContentStream;
                        auto streamData = GetStream(contentStream);
                        streamData.StreamPath = streamPath;
                        SetStream(contentStream, streamData);
                    }
                }

                // Revert faking null terminator
                if(charSet) {
                    *charSet = ';';
                }
            }
        }
    } else {
        // This is the initial http line

        char method[64];
        char url[1024];
        char version[64];

        auto parsedArgs = sscanf(headerLine, "%s %s %s", method, url, version);
        if(3 == parsedArgs) {
            auto requestData = GetHttpRequest(partial->request);
            requestData.HttpRequestMethod = method;
            requestData.HttpRequestVersion = version;

			auto queryParams = strchr(url, '?');
			if (queryParams) {
				*queryParams = '\0';
                queryParams++;
				ParseQueryParams(partial->request, requestData, queryParams);
			}

            requestData.HttpRequestUrl = url;

            SetHttpRequest(partial->request, requestData);
        }
    }

    SetHttpRequest(partial->request, requestData);
}

static void WriteResponse(Entity server, Entity clientStream, Entity request, Entity response) {

    // First, apply content headers
    ApplyResponseContentHeaders(server, request, response);

    auto responseStreamData = GetHttpStream(clientStream);
    auto responseData = GetHttpResponse(response);
    auto& responseQueue = responseStreamData.partial->responseQueue;

    char headerLine[1024];

    // Write initial HTTP response header
    auto responseCode = responseData.HttpResponseCode;
    auto responseCodeValid = IsEntityValid(responseCode);
    auto responseCodeNumber = responseCodeValid ? GetHttpResponseCode(responseCode).HttpResponseCodeNumber : 501;
    auto responseCodeMessage = responseCodeValid ? GetHttpResponseCode(responseCode).HttpResponseCodeMessage : "Not Implemented";

    snprintf(headerLine, sizeof(headerLine), "HTTP/1.1 %hu %s\r\n", responseCodeNumber, responseCodeMessage);
    responseQueue.insert(responseQueue.end(), headerLine, headerLine + strlen(headerLine));

    // Write additional HTTP headers
    for(auto header : responseData.HttpResponseHeaders) {
        auto identifier = GetHttpHeaderType(GetHttpHeader(header).HttpHeaderType).HttpHeaderTypeIdentifier;
        auto value = GetHttpHeader(header).HttpHeaderValue;

        snprintf(headerLine, sizeof(headerLine), "%s: %s\r\n", identifier, value);

        responseQueue.insert(responseQueue.end(), headerLine, headerLine + strlen(headerLine));
    }

    // Write header-to-content terminator
    strncpy(headerLine, "\r\n", sizeof(headerLine));
    responseQueue.insert(responseQueue.end(), headerLine, headerLine + strlen(headerLine));

    // Write eventual reponse content
    auto responseContentStream = GetHttpResponse(response).HttpResponseContentStream;
    if(responseCodeValid && StreamOpen(responseContentStream, StreamMode_Read)) {
        StreamSeek(responseContentStream, StreamSeek_End);
        auto contentLength = StreamTell(responseContentStream);
        StreamSeek(responseContentStream, 0);

        responseQueue.resize(responseQueue.size() + contentLength);
		auto contentData = responseQueue.data() + responseQueue.size() - contentLength;

        StreamRead(responseContentStream, contentLength, contentData);
        StreamClose(responseContentStream);
    }
}

static void OnRequestComplete(Entity server, Entity clientStream, HttpStream& streamData, HttpStreamPartialRequest *requestData) {
    auto contentStream = GetHttpRequest(requestData->request).HttpRequestContentStream;
    if(requestData->contentLength > 0 && StreamOpen(contentStream, StreamMode_Write)) {
        StreamWrite(contentStream, requestData->contentData.size(), requestData->contentData.data());
        StreamClose(contentStream);
    }

    auto response = CreateEntity();
    streamData.HttpStreamResponses.Add(response);

    auto responseData = GetHttpResponse(response);
    responseData.HttpResponseRequest = requestData->request;
    SetHttpResponse(response, responseData);

    RouteRequest(server, requestData->request, response);

    WriteResponse(server, clientStream, requestData->request, response);

    streamData.HttpStreamRequests.Remove(streamData.HttpStreamRequests.GetIndex(requestData->request));
    streamData.HttpStreamResponses.Remove(streamData.HttpStreamResponses.GetIndex(response));

    requestData->contentLength = -1;
    requestData->contentData.clear();
    requestData->headerData.clear();
}

static u64 OnHeaderData(Entity server, Entity stream, HttpStream& streamData, const char *data, u64 numBytes, bool *requestCompleted) {
    *requestCompleted = false;

    if(numBytes > 0) {
        streamData.partial->headerData.insert(streamData.partial->headerData.end(), data, data + numBytes);
    }

    auto indexOfContentTerminator = strnstr(
            streamData.partial->headerData.data(),
            "\r\n\r\n",
            streamData.partial->headerData.size());

    if(indexOfContentTerminator) {
        auto numBytesToRevert = streamData.partial->headerData.size() - (indexOfContentTerminator - streamData.partial->headerData.data()) - 4;
        streamData.partial->headerData.resize(streamData.partial->headerData.size() - numBytesToRevert);

        streamData.partial->contentLength = 0;
        streamData.partial->request = CreateEntity();

        streamData.HttpStreamRequests.Add(streamData.partial->request);

        // Parse headers
        auto processedHeaderBytes = 0;
        while(processedHeaderBytes < streamData.partial->headerData.size()) {
            auto cur = streamData.partial->headerData.data() + processedHeaderBytes;
            auto lineEnd = strstr(cur, "\r\n");
            *lineEnd = '\0';

            if(cur != lineEnd) {
                ParseHeader(server, streamData.partial, cur);
            }

            processedHeaderBytes += u64(lineEnd - cur) + 2;
        }

        // If no content-length header is provided, we have no content, so process right away.
        if(streamData.partial->contentLength == 0) {
            OnRequestComplete(server, stream, streamData, streamData.partial);
            *requestCompleted = true;
        }

        SetHttpStream(stream, streamData);

        return numBytes - numBytesToRevert;
    }

    return numBytes;
}

static u64 OnContentData(Entity server, Entity stream, HttpStream& streamData, const char *data, u64 numBytes, bool *requestCompleted) {
    *requestCompleted = false;

    if(numBytes > 0) {
        streamData.partial->contentData.insert(streamData.partial->contentData.end(), data, data + numBytes);
    }

    auto bytesBeyondContent = (s64)streamData.partial->contentData.size() - streamData.partial->contentLength;

    if(bytesBeyondContent >= 0) {
        auto numBytesToRevert = bytesBeyondContent;
        streamData.partial->headerData.resize(streamData.partial->headerData.size() - numBytesToRevert);

        OnRequestComplete(server, stream, streamData, streamData.partial);
        *requestCompleted = true;

        SetHttpStream(stream, streamData);

        return numBytes - numBytesToRevert;
    }

    return numBytes;
}

static bool OnData(Entity server, Entity stream, HttpStream& streamData, const char *data, u64 numBytes) {
    auto processedBytes = 0;
    bool requestCompleted = false;
    while(processedBytes < numBytes) {
        if(streamData.partial->contentLength >= 0) { // If content length has been determined, we are currently inside content data
            processedBytes += OnContentData(server, stream, streamData, data + processedBytes, numBytes - processedBytes, &requestCompleted);
        } else {
            processedBytes += OnHeaderData(server, stream, streamData, data + processedBytes, numBytes - processedBytes, &requestCompleted);
        }
    }
    return requestCompleted;
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    HttpServer httpServerData;
    for_entity_data(httpServer, ComponentOf_HttpServer(), &httpServerData) {
        auto tcpServerData = GetTcpServer(httpServer);
        for(auto client : tcpServerData.TcpServerClients) {

            auto streamData = GetHttpStream(client);
            auto& responseQueue = streamData.partial->responseQueue;

			if (responseQueue.size() > 0) {
				// Handle response writing
				while (responseQueue.size() > 0) {
					auto numWritten = StreamWrite(client, responseQueue.size(), responseQueue.data());
					if (numWritten == 0) {
						break;
					}

					// Remove written data from queue
					auto newSize = responseQueue.size() - numWritten;
					memmove(responseQueue.data(), responseQueue.data() + numWritten, Min(newSize, responseQueue.size() - numWritten));
					responseQueue.resize(newSize);
				}
			}
			else {
				// Handle request reading
				char readData[4096];
				while (true) {
					auto numRead = StreamRead(client, sizeof(readData), readData);

					if (numRead == 0 || OnData(httpServer, client, streamData, readData, numRead)) {
						break;
					}

					
				}
			}
        }
    }
}

static void OnTcpClientChanged(Entity client, const TcpStream& oldData, const TcpStream& newData) {
    auto server = GetOwnership(client).Owner;
    if(HasComponent(server, ComponentOf_HttpServer())) {
        AddComponent(client, ComponentOf_HttpStream());
    }
}

static void OnHttpStreamChanged(Entity entity, const HttpStream& oldData, const HttpStream& newData) {
    if(!oldData.partial && !newData.partial) {
        auto data = newData;
        data.partial = new HttpStreamPartialRequest();
        data.partial->contentLength = -1;
        SetHttpStream(entity, data);
    }

    if(oldData.partial && !newData.partial) {
        delete oldData.partial;
    }
}

static void RouteRequest(Entity server, Entity request, Entity response) {
    auto serverData = GetHttpServer(server);
    auto requestData = GetHttpRequest(request);

    for(auto route : serverData.HttpServerRoutes) {
        auto routeData = GetHttpRoute(route);

        if(routeData.HttpRouteMethod == requestData.HttpRequestMethod && memcmp(requestData.HttpRequestUrl, routeData.HttpRouteTrigger, Min(strlen(requestData.HttpRequestUrl), strlen(routeData.HttpRouteTrigger))) == 0) {
            auto functionData = GetFunction(routeData.HttpRouteFunction);

            auto routeLen = strlen(routeData.HttpRouteTrigger);
            auto urlLen = strlen(requestData.HttpRequestUrl);

            auto relativeUrl = (char*)alloca(urlLen - routeLen + 1);
            strcpy(relativeUrl, requestData.HttpRequestUrl + routeLen + 1);

            eastl::fixed_vector<Variant, 16> urlArguments;

            auto slashLoc = strchr(relativeUrl, '/');
            while(slashLoc) {
                *slashLoc = '\0';

                if(strlen(relativeUrl) > 0) {
                    relativeUrl = (char*)Intern(relativeUrl);
                    urlArguments.push_back(MakeVariant(StringRef, relativeUrl));
                }

                relativeUrl = slashLoc + 1;
                slashLoc = strchr(relativeUrl, '/');
            }

            if(urlArguments.size() == functionData.FunctionArguments.GetSize() - 3 /* minus route, request and response arguments */ ) {
                auto argumentValues = (Variant*)alloca(sizeof(Variant) * functionData.FunctionArguments.GetSize());
                argumentValues[0] = MakeVariant(Entity, route);
                argumentValues[1] = MakeVariant(Entity, request);
                argumentValues[2] = MakeVariant(Entity, response);

                for(auto j = 0; j < urlArguments.size(); ++j) {
                    auto argumentData = GetFunctionArgument(functionData.FunctionArguments[j]);
                    argumentValues[3 + j] = Cast(urlArguments[j], argumentData.FunctionArgumentType);
                }

                auto responseCode = CallFunction(routeData.HttpRouteFunction, functionData.FunctionArguments.GetSize(), argumentValues).as_Entity;

                auto responseData = GetHttpResponse(response);
                responseData.HttpResponseCode = responseCode;
                SetHttpResponse(response, responseData);

                return;
            }
        }
    }

    auto responseData = GetHttpResponse(response);
    responseData.HttpResponseCode = FindResponseCode(404);
    SetHttpResponse(response, responseData);
}

BeginUnit(HttpServer)
    BeginComponent(HttpRoute)
        RegisterProperty(StringRef, HttpRouteTrigger)
        RegisterProperty(StringRef, HttpRouteMethod)
        RegisterReferenceProperty(Function, HttpRouteFunction)
    EndComponent()

    BeginComponent(HttpServer)
        RegisterBase(TcpServer)
        RegisterArrayProperty(HttpRoute, HttpServerRoutes)
        RegisterProperty(u16, HttpServerKeepAliveTimeout)
        RegisterProperty(u16, HttpServerKeepAliveMaxConnections)
    EndComponent()

    BeginComponent(HttpStream)
        RegisterBase(TcpStream)
        RegisterArrayProperty(HttpRequest, HttpStreamRequests)
        RegisterArrayProperty(HttpResponse, HttpStreamResponses)
    EndComponent()

    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_Input)
    RegisterSystem(OnTcpClientChanged, ComponentOf_TcpStream())
    RegisterSystem(OnHttpStreamChanged, ComponentOf_HttpStream())
EndUnit()