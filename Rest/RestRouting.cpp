//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Core/Math.h>
#include <Networking/HttpServer.h>
#include "RestRouting.h"
#include "RestServer.h"

struct RestRouting {
    char RestRoutingRoute[128];
};

LocalFunction(OnHttpServerRequest, void, Entity server, Entity request) {
    auto routes = GetRestServerRoutes(server);
    for(auto i = 0; i < GetNumRestServerRoutes(server); ++i) {
        auto data = GetRestRoutingData(routes[i]);
        if(data) {
            if(memcmp(GetHttpRequestUrl(request), data->RestRoutingRoute, Min(strlen(GetHttpRequestUrl(request)), strlen(data->RestRoutingRoute))) == 0) {
                FireEvent(EventOf_RestRoutingRequest(), routes[i], request);
            }
        }
    }

    for_children(child, RestServerRoutes, server) {

    }
}

BeginUnit(RestRouting)
    BeginComponent(RestRouting)
        RegisterProperty(StringRef, RestRoutingRoute)
    EndComponent()

    RegisterEvent(RestRoutingRequest)

    RegisterSubscription(HttpServerRequest, OnHttpServerRequest, 0)
EndUnit()