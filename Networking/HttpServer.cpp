//
// Created by Kim on 15/07/2018.
//

#define USE_STANDALONE_ASIO 1
#define ASIO_STANDALONE 1
#define ASIO_NO_TYPEID 1
#define ASIO_NO_EXCEPTIONS 1
#define _WIN32_WINNT 0x0501

#define HEADER_SIZE_MAX 4096

#include <unistd.h>
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

struct HttpServer {
    u16 HttpServerKeepAliveTimeout;
    u16 HttpServerKeepAliveMaxConnections;
};

struct HttpStreamPartialRequest {
    std::vector<char> headerData;
    std::vector<char> contentData;
    s64 contentLength;
    Entity request;
    std::vector<char> responseQueue;
};

struct HttpStream {
    HttpStreamPartialRequest *partial;
};

LocalFunction(OnHttpStreamAdded, void, Entity component, Entity entity) {
    auto data = GetHttpStreamData(entity);
    data->partial = new HttpStreamPartialRequest();
    data->partial->contentLength = -1;
}

LocalFunction(OnHttpStreamRemoved, void, Entity component, Entity entity) {
    auto data = GetHttpStreamData(entity);
    delete data->partial;
}

void ApplyResponseContentHeaders(Entity server, Entity request, Entity response) {
    auto contentLengthHeader = FindHeaderType("Content-Length");
    auto contentTypeHeader = FindHeaderType("Content-Type");
    auto keepAliveHeader = FindHeaderType("Keep-Alive");
    auto connectionHeader = FindHeaderType("Connection");
    auto serverHeader = FindHeaderType("Server");

    u64 contentLength = 0;

    auto contentStream = GetHttpResponseContentStream(response);

    Entity header = 0;

    if(HasComponent(contentStream, ComponentOf_Stream())) {
        auto fileType = GetStreamFileType(contentStream);
        if(StreamOpen(contentStream, StreamMode_Read)) {
            StreamSeek(contentStream, StreamSeek_End);

            contentLength = StreamTell(contentStream);

            StreamClose(contentStream);
        }

        if(IsEntityValid(fileType)) {
            header = AddHttpResponseHeaders(response);
            SetHttpHeaderType(header, contentTypeHeader);
            SetHttpHeaderValue(header, GetFileTypeMimeType(fileType));
        }

    }

    char headerValue[16];
    snprintf(headerValue, sizeof(headerValue), "%llu", contentLength);

    header = AddHttpResponseHeaders(response);
    SetHttpHeaderType(header, contentLengthHeader);
    SetHttpHeaderValue(header, headerValue);

    for_children(requestHeader, HttpRequestHeaders, request, {

        // If we have keep-alive, apply this to response as well
        if(GetHttpHeaderType(requestHeader) == connectionHeader) {
            header = AddHttpResponseHeaders(response);
            SetHttpHeaderType(header, connectionHeader);
            SetHttpHeaderValue(header, GetHttpHeaderValue(requestHeader));

            char keepAliveHeaderValue[256];
            snprintf(keepAliveHeaderValue, sizeof(keepAliveHeaderValue), "timeout=%hu, max=%hu", GetHttpServerKeepAliveTimeout(server), GetHttpServerKeepAliveMaxConnections(server));
            header = AddHttpResponseHeaders(response);
            SetHttpHeaderType(header, keepAliveHeader);
            SetHttpHeaderValue(header, keepAliveHeaderValue);
        }
    });

    char serverHeaderValue[256];
    snprintf(serverHeaderValue, sizeof(serverHeaderValue), "%s", GetName(server));
    header = AddHttpResponseHeaders(response);
    SetHttpHeaderType(header, serverHeader);
    SetHttpHeaderValue(header, serverHeaderValue);
}

static void ParseQueryParams(Entity request, StringRef queryParams) {

}

static void ParseHeader(Entity server, HttpStreamPartialRequest *partial, char *headerLine) {
    auto colonLocation = strstr(headerLine, ": ");
    if(colonLocation) {
        // This is a http header
        *colonLocation = '\0';
        auto identifier = headerLine;
        auto value = colonLocation + 2;

        auto headerType = FindHeaderType(identifier);
        if(!headerType) {
            Log(partial->request, LogSeverity_Warning, "Unrecognized header: '%s: %s'", identifier, value);
        } else {
            auto header = AddHttpRequestHeaders(partial->request);
            SetHttpHeaderType(header, headerType);
            SetHttpHeaderValue(header, value);

            if(stricmp(identifier, "content-length") == 0) {
                partial->contentLength = strtol(value, NULL, 10);
            }

            if(stricmp(identifier, "content-type") == 0) {
                auto charSet = strstr(value, ";");

                // Fake null terminator to leave out charset separator for comparing mime type
                if(charSet) {
                    *charSet = '\0';
                }

                for_entity(fileType, fileTypeData, FileType, {
                    if(stricmp(GetFileTypeMimeType(fileType), value) == 0) {
                        char streamPath[PathMax];
                        snprintf(streamPath, PathMax, "memory://request%s", GetFileTypeExtension(fileType));

                        SetStreamPath(GetHttpRequestContentStream(partial->request), streamPath);
                    }
                });

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
            SetHttpRequestMethod(partial->request, method);
            SetHttpRequestVersion(partial->request, version);

			auto queryParams = strchr(url, '?');
			if (queryParams) {
				*queryParams = '\0';
				ParseQueryParams(partial->request, queryParams);
			}

			SetHttpRequestUrl(partial->request, url);
        }
    }
}

static void WriteResponse(Entity server, Entity clientStream, Entity request, Entity response) {

    // First, apply content headers
    ApplyResponseContentHeaders(server, request, response);

    auto responseStream = GetHttpStreamData(clientStream);
    auto& responseQueue = responseStream->partial->responseQueue;

    char headerLine[1024];

    // Write initial HTTP response header
    auto responseCode = GetHttpResponseCode(response);
    auto responseCodeValid = IsEntityValid(responseCode);
    auto responseCodeNumber = responseCodeValid ? GetHttpResponseCodeNumber(responseCode) : 501;
    auto responseCodeMessage = responseCodeValid ? GetHttpResponseCodeMessage(responseCode) : "Not Implemented";

    snprintf(headerLine, sizeof(headerLine), "HTTP/1.1 %hu %s\r\n", responseCodeNumber, responseCodeMessage);
    responseQueue.insert(responseQueue.end(), headerLine, headerLine + strlen(headerLine));

    // Write additional HTTP headers
    for_children(header, HttpResponseHeaders, response, {
        auto identifier = GetHttpHeaderTypeIdentifier(GetHttpHeaderType(header));
        auto value = GetHttpHeaderValue(header);

        snprintf(headerLine, sizeof(headerLine), "%s: %s\r\n", identifier, value);

        responseQueue.insert(responseQueue.end(), headerLine, headerLine + strlen(headerLine));
    });

    // Write header-to-content terminator
    strncpy(headerLine, "\r\n", sizeof(headerLine));
    responseQueue.insert(responseQueue.end(), headerLine, headerLine + strlen(headerLine));

    // Write eventual reponse content
    auto responseContentStream = GetHttpResponseContentStream(response);
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

static void OnRequestComplete(Entity server, Entity clientStream, HttpStreamPartialRequest *requestData) {
    auto contentStream = GetHttpRequestContentStream(requestData->request);
    if(requestData->contentLength > 0 && StreamOpen(contentStream, StreamMode_Write)) {
        StreamWrite(contentStream, requestData->contentData.size(), requestData->contentData.data());
        StreamClose(contentStream);
    }

    auto response = AddHttpServerResponses(clientStream);

	Variant values[] = { MakeVariant(Entity, server), MakeVariant(Entity, requestData->request), MakeVariant(Entity, response) };
	FireEventFast(EventOf_HttpServerRequest(), 3, values);

	auto responseData = GetHttpResponseData(response);
    WriteResponse(server, clientStream, requestData->request, response);

    RemoveHttpServerRequests(clientStream, 0);
    RemoveHttpServerResponses(clientStream, 0);

    requestData->contentLength = -1;
    requestData->contentData.clear();
    requestData->headerData.clear();
}

static u64 OnHeaderData(Entity server, Entity stream, HttpStream *streamData, const char *data, u64 numBytes, bool *requestCompleted) {
    *requestCompleted = false;

    if(numBytes > 0) {
        streamData->partial->headerData.insert(streamData->partial->headerData.end(), data, data + numBytes);
    }

    auto indexOfContentTerminator = strnstr(
            streamData->partial->headerData.data(),
            "\r\n\r\n",
            streamData->partial->headerData.size());

    if(indexOfContentTerminator) {
        auto numBytesToRevert = streamData->partial->headerData.size() - (indexOfContentTerminator - streamData->partial->headerData.data()) - 4;
        streamData->partial->headerData.resize(streamData->partial->headerData.size() - numBytesToRevert);

        streamData->partial->contentLength = 0;
        streamData->partial->request = AddHttpServerRequests(stream);

        // Parse headers
        auto processedHeaderBytes = 0;
        while(processedHeaderBytes < streamData->partial->headerData.size()) {
            auto cur = streamData->partial->headerData.data() + processedHeaderBytes;
            auto lineEnd = strstr(cur, "\r\n");
            *lineEnd = '\0';

            if(cur != lineEnd) {
                ParseHeader(server, streamData->partial, cur);
            }

            processedHeaderBytes += u64(lineEnd - cur) + 2;
        }

        // If no content-length header is provided, we have no content, so process right away.
        if(streamData->partial->contentLength == 0) {
            OnRequestComplete(server, stream, streamData->partial);
            *requestCompleted = true;
        }

        return numBytes - numBytesToRevert;
    }

    return numBytes;
}

static u64 OnContentData(Entity server, Entity stream, HttpStream *streamData, const char *data, u64 numBytes, bool *requestCompleted) {
    *requestCompleted = false;

    if(numBytes > 0) {
        streamData->partial->contentData.insert(streamData->partial->contentData.end(), data, data + numBytes);
    }

    auto bytesBeyondContent = (s64)streamData->partial->contentData.size() - streamData->partial->contentLength;

    if(bytesBeyondContent >= 0) {
        auto numBytesToRevert = bytesBeyondContent;
        streamData->partial->headerData.resize(streamData->partial->headerData.size() - numBytesToRevert);

        OnRequestComplete(server, stream, streamData->partial);
        *requestCompleted = true;

        return numBytes - numBytesToRevert;
    }

    return numBytes;
}

static bool OnData(Entity server, Entity stream, HttpStream *streamData, const char *data, u64 numBytes) {
    auto processedBytes = 0;
    bool requestCompleted = false;
    while(processedBytes < numBytes) {
        if(streamData->partial->contentLength >= 0) { // If content length has been determined, we are currently inside content data
            processedBytes += OnContentData(server, stream, streamData, data + processedBytes, numBytes - processedBytes, &requestCompleted);
        } else {
            processedBytes += OnHeaderData(server, stream, streamData, data + processedBytes, numBytes - processedBytes, &requestCompleted);
        }
    }
    return requestCompleted;
}

LocalFunction(OnAppLoopChanged, void, Entity appLoop, u64 oldFrame, u64 newFrame) {
    for_entity(httpServer, httpServerData, HttpServer, {
        for_children(client, TcpServerClients, httpServer, {

            auto streamData = GetHttpStreamData(client);
            auto& responseQueue = streamData->partial->responseQueue;

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
        });
    });
}

LocalFunction(OnTcpClientConnected, void, Entity server, Entity client) {
    if(HasComponent(server, ComponentOf_HttpServer())) {
        AddComponent(client, ComponentOf_HttpStream());
    }
}

BeginUnit(HttpServer)
    BeginComponent(HttpServer)
        RegisterBase(TcpServer)
        RegisterProperty(u16, HttpServerKeepAliveTimeout)
        RegisterProperty(u16, HttpServerKeepAliveMaxConnections)
    EndComponent()

    BeginComponent(HttpStream)
        RegisterBase(TcpStream)
        RegisterArrayProperty(HttpRequest, HttpServerRequests)
        RegisterArrayProperty(HttpResponse, HttpServerResponses)
    EndComponent()

    RegisterEvent(HttpServerRequest)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnAppLoopChanged, AppLoopOf_Networking())
    RegisterSubscription(EventOf_EntityComponentAdded(), OnHttpStreamAdded, ComponentOf_HttpStream())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnHttpStreamRemoved, ComponentOf_HttpStream())
    RegisterSubscription(EventOf_TcpClientConnected(), OnTcpClientConnected, 0)
EndUnit()