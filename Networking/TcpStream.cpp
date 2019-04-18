//
// Created by Kim on 31-07-2018.
//

#define USE_STANDALONE_ASIO 1
#define ASIO_STANDALONE 1
#define ASIO_NO_TYPEID 1
#define ASIO_NO_EXCEPTIONS 1
#define _WIN32_WINNT 0x0501

#include <asio.hpp>

#include <Foundation/Stream.h>
#include <Foundation/NativeUtils.h>
#include <Core/Algorithms.h>
#include <Foundation/AppLoop.h>
#include "TcpStream.h"
#include "Server.h"
#include "NetworkingModule.h"

using namespace asio;
using namespace asio::ip;

static asio::io_service s_io_service;

namespace asio {
    namespace detail {
        template <typename Exception>
        void throw_exception(const Exception& e) {
            //Log(0, LogSeverity_Error, "%s", e.what());
        }
    }
}

#define Verbose_TcpClient "tcpclient"

static u64 Read(Entity entity, u64 size, void *data) {
    auto streamData = GetTcpStream(entity);

    if(!((tcp::socket*)streamData.TcpStreamSocket)->is_open()) {
        return 0;
    }

    auto available = ((tcp::socket*)streamData.TcpStreamSocket)->available();
    if(available == 0) {
        return 0;
    }

    error_code ec;
    size = asio::read(*((tcp::socket*)streamData.TcpStreamSocket), asio::buffer(data, size), transfer_at_least(available), ec);

    if(ec == asio::error::eof) {
        return 0;
    }

    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    auto streamData = GetTcpStream(entity);

    if(!((tcp::socket*)streamData.TcpStreamSocket)->is_open()) {
        return 0;
    }

	error_code ec;
    size = asio::write(*((tcp::socket*)streamData.TcpStreamSocket), asio::const_buffer(data, size), asio::transfer_at_least(1), ec);

	if (ec == asio::error::eof) {
		return 0;
	}

	char lastChar = ((const char*)data)[size - 1];

    return size;
}

static s32 Tell(Entity entity) {
    auto streamData = GetTcpStream(entity);
    return streamData.readOffset;
}

static bool Seek(Entity entity, s32 offset) {
    return false;
}

static bool Open(Entity entity, int modeFlag) {
    auto streamData = GetTcpStream(entity);

    if(((tcp::socket*)streamData.TcpStreamSocket)) return true;

    char protocol[128];
    char host[256];
    char port[128];

    auto numParsed = sscanf(GetStream(entity).StreamPath, "%s://%s:%s", protocol, host, port);
    if(numParsed != 3) {
        Log(entity, LogSeverity_Error, "Incorrect TCP stream path format. Should be: tcp://address:port");
        return false;
    }

    tcp::resolver resolver(s_io_service);
    tcp::resolver::query query(host, port);
    tcp::resolver::iterator iter = resolver.resolve(query), end;

    auto socket = new tcp::socket(s_io_service);
    asio::error_code error = asio::error::host_not_found;
    while (error && iter != end)
    {
       socket->close();
       socket->connect(*iter++, error);
    }

    if (error) {
        delete socket;
        return false;
    }

    streamData.TcpStreamSocket = socket;
    SetTcpStream(entity, streamData);

    return true;
}

static bool Close(Entity entity) {
    auto data = GetTcpStream(entity);

    if(data.TcpStreamSocket) {
        ((tcp::socket*)data.TcpStreamSocket)->close();
        data.TcpStreamSocket = NULL;
        SetTcpStream(entity, data);
        return true;
    }

    return false;
}

static bool IsOpen(Entity entity) {
    auto data = GetTcpStream(entity);

    if(data.TcpStreamSocket && !((tcp::socket*)data.TcpStreamSocket)->is_open()) {
        data.TcpStreamSocket = NULL;
        SetTcpStream(entity, data);
    }

    return data.TcpStreamSocket != NULL;
}

static bool Delete(Entity entity) {
    return false;
}

static void StartAccept(Entity server) {
    auto data = GetTcpServer(server);

    auto socket = new tcp::socket(s_io_service);
    ((tcp::acceptor*)data.TcpServerAcceptor)->async_accept(*socket, [server, socket](const asio::error_code& e) {
        auto tcpServerData = GetTcpServer(server);
        StartAccept(server);

        auto stream = CreateEntity();
        tcpServerData.TcpServerClients.Add(stream);

        auto tcpStreamData = GetTcpStream(stream);
        tcpStreamData.TcpStreamSocket = socket;

        auto remoteIp = "tcp://" + socket->remote_endpoint().address().to_string() + "/";

        auto streamData = GetStream(stream);
        streamData.StreamPath = remoteIp.c_str();
        SetStream(stream, streamData);
        SetTcpStream(stream, tcpStreamData);

        StreamOpen(stream, StreamMode_Read | StreamMode_Write);

        Verbose(Verbose_TcpClient, "Client %u connected: %s", GetComponentIndex(ComponentOf_TcpStream(), stream), GetStreamPath(stream));

        SetTcpServer(server, tcpServerData);
    });
}

static void OnTcpStreamChanged(Entity entity, const TcpStream& oldData, const TcpStream& newData) {
    if(oldData.TcpStreamSocket && !newData.TcpStreamSocket) {
        delete ((tcp::socket*)oldData.TcpStreamSocket);
    }
}

static void OnServerChanged(Entity entity, const Server& oldData, const Server& newData) {
    auto tcpServerData = GetTcpServer(entity);
    if(!newData.ServerPort && tcpServerData.TcpServerAcceptor) {
        tcpServerData.TcpServerAcceptor = NULL;
        SetTcpServer(entity, tcpServerData);
    }
}

static void OnTcpServerChanged(Entity server, const TcpServer& oldData, const TcpServer& newData) {
    if(oldData.TcpServerAcceptor && !newData.TcpServerAcceptor) {
        auto acceptor = ((tcp::acceptor*)oldData.TcpServerAcceptor);
        acceptor->close();
        delete acceptor;
    }

    if(!oldData.TcpServerAcceptor && !newData.TcpServerAcceptor) {
        auto data = newData;
        auto acceptor = new tcp::acceptor(s_io_service, ip::tcp::endpoint{ip::tcp::v4(), GetServer(server).ServerPort});
        acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor->set_option(asio::ip::tcp::acceptor::keep_alive(true));
        acceptor->listen();
        data.TcpServerAcceptor = acceptor;
        SetTcpServer(server, data);

        StartAccept(server);
    }
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    if(s_io_service.poll() == 0 && GetNetworking(ModuleOf_Networking()).TcpWaitOnNoWork) {
#ifdef LINUX
        usleep(1);
#endif
#if defined(_WIN32) || defined(_WIN64)
        Sleep(1);
#endif
    }

    // Remove closed clients from server
    TcpServer serverData;
    for_entity_data(server, ComponentOf_TcpServer(), &serverData) {
        for(auto i = 0; i < serverData.TcpServerClients.GetSize(); ++i) {
            auto client = serverData.TcpServerClients[i];
            if(!IsOpen(client)) {
				Verbose(Verbose_TcpClient, "Client %u disconnected: %s", GetComponentIndex(ComponentOf_TcpStream(), client), GetStreamPath(client));
                serverData.TcpServerClients.Remove(i);
                i--;
            }
        }
    }
}

BeginUnit(TcpStream)
    BeginComponent(TcpStream)
        RegisterBase(Stream)
    EndComponent()

    BeginComponent(TcpServer)
        RegisterBase(Server)
        RegisterArrayProperty(TcpStream, TcpServerClients)
    EndComponent()

    RegisterSystem(OnTcpStreamChanged, ComponentOf_TcpStream())
    RegisterSystem(OnServerChanged, ComponentOf_Server())
    RegisterSystem(OnTcpServerChanged, ComponentOf_TcpServer())
    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_Input)

    RegisterStreamProtocol(Tcp, "tcp")
EndComponent()
