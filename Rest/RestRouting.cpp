//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Core/Algorithms.h>
#include <Networking/HttpServer.h>
#include "RestRouting.h"
#include "RestServer.h"

struct RestRouting {
    StringRef RestRoutingRoute;
};

LocalFunction(OnHttpServerRequest, void, Entity server, Entity request, Entity response) {
    auto& routes = GetRestServerRoutes(server);
    for(auto i = 0; i < routes.size(); ++i) {
        auto data = GetRestRoutingData(routes[i]);
        if(data) {
            auto requestUrl = GetHttpRequestUrl(request);
            if(memcmp(GetHttpRequestUrl(request), data->RestRoutingRoute, Min(strlen(requestUrl), strlen(data->RestRoutingRoute))) == 0) {

				Variant values[] = { MakeVariant(Entity, routes[i]), MakeVariant(Entity, request), MakeVariant(Entity, response) };
				FireEventFast(EventOf_RestRoutingRequest(), 3, values);
            }
        }
    }
}

BeginUnit(RestRouting)
    BeginComponent(RestRouting)
        RegisterProperty(StringRef, RestRoutingRoute)
    EndComponent()

    RegisterEvent(RestRoutingRequest)

    RegisterSubscription(EventOf_HttpServerRequest(), OnHttpServerRequest, 0)
EndUnit()