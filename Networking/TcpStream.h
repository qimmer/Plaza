//
// Created by Kim on 31-07-2018.
//

#ifndef PLAZA_HTTPSTREAM_H
#define PLAZA_HTTPSTREAM_H

#include <Core/NativeUtils.h>

Unit(TcpStream)
    Component(TcpStream)
    Component(TcpServer)
        ArrayProperty(TcpStream, TcpServerClients)

    Event(TcpClientConnected, Entity server, Entity clientStream)

#endif //PLAZA_HTTPSTREAM_H
