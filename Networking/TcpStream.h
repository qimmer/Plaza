//
// Created by Kim on 31-07-2018.
//

#ifndef PLAZA_HTTPSTREAM_H
#define PLAZA_HTTPSTREAM_H

#include <Core/NativeUtils.h>
#include <Foundation/NativeUtils.h>

struct TcpStream {
    NativePtr TcpStreamSocket;
    u64 readOffset;
};

struct TcpServer {
    ChildArray TcpServerClients;
    NativePtr TcpServerAcceptor;
};

Unit(TcpStream)
    Component(TcpStream)
        Property(NativePtr, TcpStreamSocket)

    Component(TcpServer)
        Property(NativePtr, TcpServerAcceptor)
        ArrayProperty(TcpStream, TcpServerClients)

    StreamProtocol(Tcp)

#endif //PLAZA_HTTPSTREAM_H
