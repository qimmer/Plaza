//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_HTTPSERVER_H
#define PLAZA_HTTPSERVER_H

#include <Core/NativeUtils.h>

Unit(HttpServer)
    Component(HttpServer)
        ArrayProperty(HttpRequest, HttpServerRequests)

    Event(HttpServerRequest, Entity request)

#endif //PLAZA_HTTPSERVER_H
