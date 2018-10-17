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


namespace asio {
    namespace detail {
        template <typename Exception>
        void throw_exception(const Exception& e) {
            //Log(0, LogSeverity_Error, "%s", e.what());
        }
    }
}

#endif //PLAZA_HTTPSTREAM_H
