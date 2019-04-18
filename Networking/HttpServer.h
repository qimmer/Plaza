//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_HTTPSERVER_H
#define PLAZA_HTTPSERVER_H

#include <Core/NativeUtils.h>

struct HttpRoute {
    StringRef HttpRouteTrigger;
    StringRef HttpRouteMethod;
    Entity HttpRouteFunction;
};

struct HttpServer {
    u16 HttpServerKeepAliveTimeout;
    u16 HttpServerKeepAliveMaxConnections;
    ChildArray HttpServerStreams;
    ChildArray HttpServerRoutes;
};

struct HttpStream {
    struct HttpStreamPartialRequest *partial;
    ChildArray HttpStreamRequests, HttpStreamResponses;
};

Unit(HttpServer)
    Component(HttpServer)
        Property(u16, HttpServerKeepAliveTimeout)
        Property(u16, HttpServerKeepAliveMaxConnections)
        ArrayProperty(HttpStream, HttpServerStreams)
        ArrayProperty(Route, HttpServerRoutes)

    Component(HttpStream)
        ArrayProperty(HttpRequest, HttpStreamRequests)
        ArrayProperty(HttpResponse, HttpStreamResponses)

    Component(HttpRoute)
        Property(StringRef, HttpRouteTrigger)
        Property(StringRef, HttpRouteMethod)
        ReferenceProperty(Function, HttpRouteFunction)

#endif //PLAZA_HTTPSERVER_H
