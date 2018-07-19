//
// Created by Kim on 15/07/2018.
//

#define USE_STANDALONE_ASIO 1
#define ASIO_STANDALONE 1
#define ASIO_NO_TYPEID 1
#define ASIO_NO_EXCEPTIONS 1
#define _WIN32_WINNT 0x0501

#include <asio.hpp>
#include <unordered_map>
#include <chrono>
#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>

#include "HttpServer.h"
#include "HttpRequest.h"
#include "Server.h"

using namespace asio;

template <typename Exception>
void asio::detail::throw_exception(const Exception& e) {
    Log(0, LogSeverity_Error, "Http Error: %s", e.what());
}

struct RequestData
{
    std::string method;
    std::string url;
    std::string version;

    std::unordered_map<std::string, std::string> headers;

    int content_length()
    {
        auto request = headers.find("content-length");
        if(request != headers.end())
        {
            std::stringstream ssLength(request->second);
            int content_length;
            ssLength >> content_length;
            return content_length;
        }
        return 0;
    }

    void on_read_header(std::string line)
    {
        std::stringstream ssHeader(line);
        std::string headerName;
        std::getline(ssHeader, headerName, ':');

        std::string value;
        std::getline(ssHeader, value);
        headers[headerName] = value;
    }

    void on_read_request_line(std::string line)
    {
        std::stringstream ssRequestLine(line);
        ssRequestLine >> method;
        ssRequestLine >> url;
        ssRequestLine >> version;
    }
};

class RequestSession
{
public:
    asio::streambuf buff;
    RequestData headers;
    ip::tcp::socket socket;

    RequestSession(io_service& io_service)
            :socket(io_service)
    {
    }
};


static void HandleResponse(Entity server, RequestSession *data, std::stringstream& ssOut) {
    auto request = AddHttpServerRequests(server);
    SetHttpRequestMethod(request, data->headers.method.c_str());
    SetHttpRequestUrl(request, data->headers.url.c_str());
    SetHttpRequestVersion(request, data->headers.version.c_str());

    FireEvent(EventOf_HttpServerRequest(), server, request);

    auto response = GetHttpRequestResponse(request);
    Entity streamFileType = GetStreamFileType(response);
    if(HasComponent(response, ComponentOf_Stream()) && StreamOpen(response, StreamMode_Read)) {
        StreamSeek(response, StreamSeek_End);
        auto size = StreamTell(response);
        StreamSeek(response, 0);

        auto streamData = (char*)malloc(size+1);
        StreamRead(response, size, streamData);
        StreamClose(response);

        streamData[size] = '\0';

        ssOut << "HTTP/1.1 " << GetHttpResponseCode(response) << " OK" << std::endl;

        ssOut << "Access-Control-Allow-Origin: *" << std::endl;
        ssOut << "Access-Control-Allow-Credentials: true" << std::endl;
        ssOut << "Access-Control-Allow-Headers: Content-Type, Accept, X-Requested-With, remember-me" << std::endl;

        ssOut << "content-type: " << (IsEntityValid(streamFileType) ? GetFileTypeMimeType(streamFileType) : "application/octet-stream") << std::endl;
        ssOut << "content-length: " << size << std::endl;
        ssOut << std::endl;

        ssOut.write(streamData, size);

        free(streamData);
    } else {
        ssOut << "HTTP/1.1 404 Not Found" << std::endl;
        ssOut << "content-length: 0" << std::endl;
        ssOut << std::endl;
    }

    auto requests = GetHttpServerRequests(server);
    for(auto i = 0; i < GetNumHttpServerRequests(server); ++i) {
        if(requests[i] == request) {
            RemoveHttpServerRequests(server, i);
            break;
        }
    }
}


static void read_body(std::shared_ptr<RequestSession> pThis, Entity server)
{
    int nbuffer = pThis->headers.content_length();
    std::shared_ptr<std::vector<char>> bufptr = std::make_shared<std::vector<char>>(nbuffer);

    asio::async_read(pThis->socket, asio::buffer(*bufptr, nbuffer),
                     [pThis, server](const error_code& e, std::size_t s)
                     {
                         std::stringstream ssOut;
                         HandleResponse(server, pThis.get(), ssOut);
                         auto str = ssOut.str();
                         asio::async_write(pThis->socket, asio::buffer(str.c_str(), str.length()), [pThis, str](const error_code& e, std::size_t s) {});
                     });
}

static void read_next_line(std::shared_ptr<RequestSession> pThis, Entity server)
{
    asio::async_read_until(pThis->socket, pThis->buff, '\r', [pThis, server](const error_code& e, std::size_t s) {
        std::string line, ignore;
        std::istream stream {&pThis->buff};
        std::getline(stream, line, '\r');
        std::getline(stream, ignore, '\n');
        pThis->headers.on_read_header(line);

        if(line.length() == 0)
        {
            if(pThis->headers.content_length() == 0)
            {
                std::stringstream ssOut;
                HandleResponse(server, pThis.get(), ssOut);
                auto str = ssOut.str();
                asio::write(pThis->socket, asio::buffer(str.c_str(), str.length()));
            }
            else
            {
                read_body(pThis, server);
            }
        }
        else
        {
            read_next_line(pThis, server);
        }
    });
}

static void read_first_line(std::shared_ptr<RequestSession> pThis, Entity server)
{
    asio::async_read_until(pThis->socket, pThis->buff, '\r',
                           [pThis, server](const error_code& e, std::size_t s)
                           {
                               std::string line, ignore;
                               std::istream stream {&pThis->buff};
                               std::getline(stream, line, '\r');
                               std::getline(stream, ignore, '\n');
                               pThis->headers.on_read_request_line(line);
                               read_next_line(pThis, server);
                           });
}

static void interact(std::shared_ptr<RequestSession> pThis, Entity server)
{
    read_first_line(pThis, server);
}

void accept_and_run(ip::tcp::acceptor& acceptor, io_service& io_service, Entity server)
{
    std::shared_ptr<RequestSession> sesh = std::make_shared<RequestSession>(io_service);
    acceptor.async_accept(sesh->socket, [sesh, &acceptor, &io_service, server](const error_code& accept_error) {
        accept_and_run(acceptor, io_service, server);
        if(!accept_error)
        {
            interact(sesh, server);
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

        httpServerData->impl->io_service.run_for(chrono::milliseconds(10));
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