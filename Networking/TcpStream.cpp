//
// Created by Kim on 31-07-2018.
//

#define USE_STANDALONE_ASIO 1
#define ASIO_STANDALONE 1
#define ASIO_NO_TYPEID 1
#define ASIO_NO_EXCEPTIONS 1
#define _WIN32_WINNT 0x0501

#include <unistd.h>
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


#define Verbose_TcpClient "tcpclient"

struct TcpStream {
    tcp::socket *socket;
    u64 readOffset;
};

struct TcpServer {
    tcp::acceptor *acceptor;
};

static u64 Read(Entity entity, u64 size, void *data) {
    auto streamData = GetTcpStreamData(entity);

    if(!streamData->socket->is_open()) {
        return 0;
    }

    auto available = streamData->socket->available();
    if(available == 0) {
        return 0;
    }

    error_code ec;
    size = asio::read(*streamData->socket, asio::buffer(data, size), transfer_at_least(available), ec);

    if(ec == asio::error::eof) {
        return 0;
    }

    return size;
}

static u64 Write(Entity entity, u64 size, const void *data) {
    auto streamData = GetTcpStreamData(entity);

    if(!streamData->socket->is_open()) {
        return 0;
    }

	error_code ec;
    size = asio::write(*streamData->socket, asio::const_buffer(data, size), asio::transfer_at_least(1), ec);

	if (ec == asio::error::eof) {
		return 0;
	}

	char lastChar = ((const char*)data)[size - 1];

    return size;
}

static s32 Tell(Entity entity) {
    auto streamData = GetTcpStreamData(entity);
    return streamData->readOffset;
}

static bool Seek(Entity entity, s32 offset) {
    return false;
}

static bool Open(Entity entity, int modeFlag) {
    auto streamData = GetTcpStreamData(entity);

    if(streamData->socket) return true;

    char protocol[128];
    char host[256];
    char port[128];

    auto numParsed = sscanf(GetStreamPath(entity), "%s://%s:%s", protocol, host, port);
    if(numParsed != 3) {
        Log(entity, LogSeverity_Error, "Incorrect TCP stream path format. Should be: tcp://address:port");
        return false;
    }

    tcp::resolver resolver(s_io_service);
    tcp::resolver::query query(host, port);
    tcp::resolver::iterator iter = resolver.resolve(query), end;

    streamData->socket = new tcp::socket(s_io_service);
    asio::error_code error = asio::error::host_not_found;
    while (error && iter != end)
    {
        streamData->socket->close();
        streamData->socket->connect(*iter++, error);
    }

    if (error) {
        delete streamData->socket;
        streamData->socket = NULL;
        return false;
    }

    return true;
}

static bool Close(Entity entity) {
    auto data = GetTcpStreamData(entity);

    if(data && data->socket) {
        data->socket->close();
        delete data->socket;
        data->socket = NULL;
        return true;
    }

    return false;
}

static bool IsOpen(Entity entity) {
    auto data = GetTcpStreamData(entity);

    if(data->socket && !data->socket->is_open()) {
        delete data->socket;
        data->socket = NULL;
    }

    return data->socket != NULL;
}

static bool Delete(Entity entity) {
    return false;
}

LocalFunction(OnTcpStreamRemoved, void, Entity component, Entity entity) {
    Close(entity);
}

LocalFunction(OnTcpServerRemoved, void, Entity component, Entity entity) {
    SetServerPort(entity, 0); // Trigger disposal of server
}

static void StartAccept(Entity server) {
    auto data = GetTcpServerData(server);

    auto socket = new tcp::socket(s_io_service);
    data->acceptor->async_accept(*socket, [server, socket](const asio::error_code& e) {
        StartAccept(server);

        auto stream = AddTcpServerClients(server);
        AddComponent(stream, ComponentOf_TcpStream());
        auto streamData = GetTcpStreamData(stream);

        streamData->socket = socket;

        auto remoteIp = "tcp://" + socket->remote_endpoint().address().to_string() + "/";

        SetStreamPath(stream, remoteIp.c_str());

        StreamOpen(stream, StreamMode_Read | StreamMode_Write);

        Verbose(Verbose_TcpClient, "Client %u connected: %s", GetComponentIndex(ComponentOf_TcpStream(), stream), GetStreamPath(stream));

		Variant values[] = { MakeVariant(Entity, server), MakeVariant(Entity, stream) };
		FireEventFast(EventOf_TcpClientConnected(), 2, values);
    });
}

LocalFunction(OnTcpServerPortChanged, void, Entity server, u16 oldPort, u16 newPort) {
    auto data = GetTcpServerData(server);

    if(data) {
        if(oldPort && data->acceptor) {
            data->acceptor->close();
            delete data->acceptor;
            data->acceptor = NULL;
        }

        if(newPort) {
            data->acceptor = new tcp::acceptor(s_io_service, ip::tcp::endpoint{ip::tcp::v4(), newPort});
            data->acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));
            data->acceptor->set_option(asio::ip::tcp::acceptor::keep_alive(true));
            data->acceptor->listen();

            StartAccept(server);
        }
    }
}

LocalFunction(OnAppLoopFrameChanged, void, Entity appLoop, u64 oldFrame, u64 newFrame) {
    if(s_io_service.poll() == 0 && GetTcpWaitOnNoWork(ModuleOf_Networking())) {
#ifdef LINUX
        usleep(1);
#endif
#if defined(_WIN32) || defined(_WIN64)
        Sleep(1);
#endif
    }

    // Remove closed clients from server
    for_entity(server, serverData, TcpServer, {
        u32 count = 0;
        auto clients = GetTcpServerClients(server, &count);
        for(auto i = 0; i < count; ++i) {
            auto client = clients[i];
            if(!IsOpen(client)) {
				Verbose(Verbose_TcpClient, "Client %u disconnected: %s", GetComponentIndex(ComponentOf_TcpStream(), client), GetStreamPath(client));
                RemoveTcpServerClients(server, i);
                i--;
            }
        }
    });
}

BeginUnit(TcpStream)
    BeginComponent(TcpStream)
        RegisterBase(Stream)
    EndComponent()

    BeginComponent(TcpServer)
        RegisterBase(Server)
        RegisterArrayProperty(TcpStream, TcpServerClients)
    EndComponent()

    RegisterEvent(TcpClientConnected)

    RegisterSubscription(EventOf_EntityComponentRemoved(), OnTcpStreamRemoved, ComponentOf_TcpStream())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ServerPort()), OnTcpServerPortChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnAppLoopFrameChanged, 0)

    RegisterStreamProtocol(TcpStream, "tcp")
EndComponent()
