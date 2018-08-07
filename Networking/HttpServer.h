//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_HTTPSERVER_H
#define PLAZA_HTTPSERVER_H

#include <Core/NativeUtils.h>

Unit(HttpServer)
    Component(HttpServer)
        ArrayProperty(HttpRequest, HttpServerRequests)
        ArrayProperty(HttpResponse, HttpServerResponses)
        Property(u16, HttpServerKeepAliveTimeout)
        Property(u16, HttpServerKeepAliveMaxConnections)

    Component(HttpStream)

    Event(HttpServerRequest, Entity request, Entity response)

#endif //PLAZA_HTTPSERVER_H
