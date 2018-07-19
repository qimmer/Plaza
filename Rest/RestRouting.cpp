//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Core/Math.h>
#include <Networking/HttpServer.h>
#include "RestRouting.h"

struct RestRouting {
    char RestRoutingRoute[128];
};

LocalFunction(OnHttpServerRequest, void, Entity server, Entity request) {
    for_children(child, server) {
        auto data = GetRestRoutingData(child);
        if(data) {
            if(memcmp(GetHttpRequestUrl(request), data->RestRoutingRoute, Min(strlen(GetHttpRequestUrl(request)), strlen(data->RestRoutingRoute))) == 0) {
                FireEvent(EventOf_RestRoutingRequest(), child, request);
            }
        }
    }
}

BeginUnit(RestRouting)
    BeginComponent(RestRouting)
        RegisterProperty(StringRef, RestRoutingRoute)
    EndComponent()

    RegisterEvent(RestRoutingRequest)

    RegisterSubscription(HttpServerRequest, OnHttpServerRequest, 0)
EndUnit()