//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include <Json/JsonPersistance.h>
#include "RestEntityRouting.h"
#include "RestRouting.h"

struct RestEntityRouting {
    Entity RestEntityRoutingRoot;
};

LocalFunction(OnRestRoutingRequest, void, Entity routing, Entity request) {
    auto data = GetRestEntityRoutingData(routing);

    if(data && IsEntityValid(data->RestEntityRoutingRoot)) {
        char completeRoute[1024];
        auto requestUrl = GetHttpRequestUrl(request);
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing));

        snprintf(completeRoute, 1024, "%s%s", GetEntityPath(data->RestEntityRoutingRoot), relativeUrl);
        auto requestedEntity = FindEntityByPath(completeRoute);

        if(!IsEntityValid(requestedEntity)) return;

        auto response = CreateEntityFromName(request, "Response");
        SetStreamPath(response, "memory://response.json");
        SetHttpResponseCode(response, 200);

        SerializeJson(response, requestedEntity);

        SetHttpRequestResponse(request, response);
    }
}

BeginUnit(RestEntityRouting)
    BeginComponent(RestEntityRouting)
        RegisterBase(RestRouting)
        RegisterProperty(Entity, RestEntityRoutingRoot)
    EndComponent()

    RegisterSubscription(RestRoutingRequest, OnRestRoutingRequest, 0)
EndUnit()