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
#include <Core/Math.h>

#include "HttpServer.h"
#include "HttpRequest.h"
#include "Server.h"

using namespace asio;

template <typename Exception>
void asio::detail::throw_exception(const Exception& e) {
    Log(0, LogSeverity_Error, "%s", e.what());
}

static std::unordered_map<int, StringRef> responseCodeNames = {
    { 200, "OK" },
    { 201, "Created" },
    { 202, "Accepted" },
    { 204, "No Content" },

    { 301, "Moved Permanently" },
    { 302, "Found" },
    { 303, "See Other" },
    { 304, "Not Modified" },
    { 307, "Temporary Redirect" },

    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
    { 406, "Not Acceptable" },
    { 412, "Precondition Failed" },
    { 415, "Unsupported Media Type" },

    { 500, "Internal Server Error" },
    { 501, "Not Implemented" }
};

struct RequestSession
{
    asio::streambuf requestHeadersBuffer;

    std::unordered_map<std::string, std::string> headerData;
    std::string url;
    std::string method;
    std::string httpVersion;
    std::vector<char> contentData, responseData;

    ip::tcp::socket socket;

    RequestSession(io_service& io_service)
            :socket(io_service)
    {
    }
};


static void HandleResponse(Entity server, Entity request, std::shared_ptr<RequestSession> pThis) {
    auto response = GetHttpRequestResponse(request);

    Entity streamFileType = GetStreamFileType(response);
    auto responseCode = GetHttpResponseCode(response);
    StringRef responseCodeName = 0;

    if(responseCode == 0) {
        responseCode = 400;
    }

    auto responseNameIt = responseCodeNames.find(responseCode);
    if(responseNameIt == responseCodeNames.end()) {
        responseCodeName = "Unknown";
    } else {
        responseCodeName = responseNameIt->second;
    }

    if(HasComponent(response, ComponentOf_Stream()) && StreamOpen(response, StreamMode_Read)) {
        StreamSeek(response, StreamSeek_End);
        auto contentLength = StreamTell(response);
        StreamSeek(response, 0);

        char headerData[2048];
        snprintf(headerData, 2048,
            "HTTP/1.1 %d %s\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "Access-Control-Allow-Headers: Content-Type, Accept, X-Requested-With, remember-me\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %d\r\n"
            "\r\n",
                 responseCode, responseCodeName,
                 (IsEntityValid(streamFileType) ? GetFileTypeMimeType(streamFileType) : "application/octet-stream"),
                 contentLength
        );

        auto headerLength = strlen(headerData);
        pThis->responseData.resize(headerLength + contentLength);

        memcpy(pThis->responseData.data(), headerData, headerLength);

        StreamRead(response, contentLength, pThis->responseData.data() + headerLength);
        StreamClose(response);
    } else {
        char headerData[2048];
        snprintf(headerData, 2048,
                 "HTTP/1.1 %d %s\r\n"
                 "Content-Length: %d\r\n"
                 "\r\n",
                 responseCode, responseCodeName,
                 0
        );

        auto headerLength = strlen(headerData);

        pThis->responseData.resize(headerLength);
        memcpy(pThis->responseData.data(), headerData, headerLength);
    }

    auto requests = GetHttpServerRequests(server);
    for(auto i = 0; i < GetNumHttpServerRequests(server); ++i) {
        if(requests[i] == request) {
            RemoveHttpServerRequests(server, i);
            break;
        }
    }

    asio::async_write(pThis->socket, asio::const_buffer(pThis->responseData.data(), pThis->responseData.size()), [pThis](const error_code& e, std::size_t s){
        pThis->socket.close();
    });
}


static void HandleRequest(std::shared_ptr<RequestSession> pThis, Entity server)
{
    auto request = AddHttpServerRequests(server);

    asio::async_read_until(
            pThis->socket,
            pThis->requestHeadersBuffer,
            "\r\n\r\n",
            [pThis, server, request](const error_code& e, std::size_t s)
            {
                Assert(server, s < HEADER_SIZE_MAX);

                std::istream is(&pThis->requestHeadersBuffer);

                // Parse initial HTTP line
                std::getline(is, pThis->method, ' ');
                std::getline(is, pThis->url, ' ');

                std::replace(pThis->url.begin(), pThis->url.end(), '?', '\0'); // End url at eventual query parameter list start

                std::getline(is, pThis->httpVersion, '\r');
                is.get(); // \n

                SetHttpRequestMethod(request, pThis->method.c_str());
                SetHttpRequestUrl(request, pThis->url.c_str());
                SetHttpRequestVersion(request, pThis->httpVersion.c_str());

                // Parse n number of header lines
                std::string headerName, headerValue;
                while(is.peek() != '\r' && !is.eof()) {
                    std::getline(is, headerName, ':');

                    is.get(); // space
                    std::getline(is, headerValue, '\r');

                    is.get(); // \n

                    std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
                    pThis->headerData[headerName] = headerValue;

                    if(headerName == "content-length") {
                        pThis->contentData.resize(strtol(headerValue.c_str(), NULL, 10));
                    }

                    if(headerName == "content-type") {
                        std::string mimeType, charSet;

                        std::istringstream iss(headerValue);
                        std::getline(iss, mimeType, ';');
                        std::getline(iss, charSet, ';');

                        for_entity(fileType, fileTypeData, FileType) {
                            if(strcmp(GetFileTypeMimeType(fileType), mimeType.c_str()) == 0) {

                                char streamPath[PathMax];
                                snprintf(streamPath, PathMax, "memory://request%s", GetFileTypeExtension(fileType));

                                SetStreamPath(request, streamPath);
                            }
                        }
                    }
                }

                is.get(); // \r
                is.get(); // \n

                auto remainingBytes = pThis->contentData.size() - Min(s, pThis->contentData.size());

                asio::async_read(pThis->socket, pThis->requestHeadersBuffer, asio::transfer_at_least(remainingBytes), [pThis, server, request](const error_code& e, std::size_t s) {
                    pThis->requestHeadersBuffer.sgetn(pThis->contentData.data(), pThis->contentData.size());

                    if(StreamOpen(request, StreamMode_Write)) {
                        StreamWrite(request, pThis->contentData.size(), pThis->contentData.data());
                        StreamClose(request);
                    }

                    FireEvent(EventOf_HttpServerRequest(), server, request);

                    HandleResponse(server, request, pThis);
                });
            });
}

void accept_and_run(ip::tcp::acceptor& acceptor, io_service& io_service, Entity server)
{
    std::shared_ptr<RequestSession> sesh = std::make_shared<RequestSession>(io_service);
    acceptor.async_accept(sesh->socket, [sesh, &acceptor, &io_service, server](const error_code& accept_error) {
        accept_and_run(acceptor, io_service, server);
        if(!accept_error)
        {
            HandleRequest(sesh, server);
        }
    });
}

struct HttpServerImpl {
    asio::io_service io_service;
    ip::tcp::acceptor acceptor;

    HttpServerImpl(u16 port, Entity server) : acceptor(io_service, ip::tcp::endpoint{ip::tcp::v4(), port}) {
        acceptor.listen();

        accept_and_run(acceptor, io_service, server);
    }

    ~HttpServerImpl() {
        acceptor.close();
        io_service.stop();
    }
};

struct HttpServer {
    Vector(HttpServerRequests, Entity, 128)
    HttpServerImpl *impl;
};

LocalFunction(OnServerPortChanged, void, Entity server, u16 oldPort, u16 newPort) {
    if(HasComponent(server, ComponentOf_HttpServer())) {
        auto data = GetHttpServerData(server);

        if(data->impl) {
            delete data->impl;
            data->impl = 0;
        }

        if(newPort > 0) {
            data->impl = new HttpServerImpl(newPort, server);
        }
    }
}

LocalFunction(OnHttpServerAdded, void, Entity server) {
    OnServerPortChanged(server, 0, GetServerPort(server));
}

LocalFunction(OnAppLoopChanged, void, Entity appLoop, u64 oldFrame, u64 newFrame) {
    for_entity(httpServer, httpServerData, HttpServer) {
        if(!httpServerData->impl) continue;

        httpServerData->impl->io_service.run_for(chrono::milliseconds(1));
    }
}

BeginUnit(HttpServer)
    BeginComponent(HttpServer)
        RegisterBase(Server)
        RegisterArrayProperty(HttpRequest, HttpServerRequests)
    EndComponent()

    RegisterEvent(HttpServerRequest)
    RegisterSubscription(AppLoopFrameChanged, OnAppLoopChanged, 0)
    RegisterSubscription(EntityComponentAdded, OnHttpServerAdded, ComponentOf_HttpServer())
    RegisterSubscription(ServerPortChanged, OnServerPortChanged, 0)
EndUnit()