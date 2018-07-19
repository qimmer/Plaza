//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include "RestResourceRouting.h"
#include "RestRouting.h"

struct RestResourceRouting {
    char RestResourceRoutingRoot[256];
    char RestResourceRoutingDefaultFile[64];
};

LocalFunction(OnRestRoutingRequest, void, Entity routing, Entity request) {
    auto data = GetRestResourceRoutingData(routing);

    if(data) {
        char completeRoute[1024];
        auto requestUrl = GetHttpRequestUrl(request);
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing));
        snprintf(completeRoute, 1024, "%s/%s", data->RestResourceRoutingRoot, relativeUrl);

        auto response = CreateEntityFromName(request, "Response");
        SetStreamPath(response, completeRoute);
        SetHttpResponseCode(response, 200);
        SetHttpRequestResponse(request, response);
    }
}

BeginUnit(RestResourceRouting)
    BeginComponent(RestResourceRouting)
        RegisterBase(RestRouting)
        RegisterProperty(StringRef, RestResourceRoutingRoot)
        RegisterProperty(StringRef, RestResourceRoutingDefaultFile)
    EndComponent()

    RegisterSubscription(RestRoutingRequest, OnRestRoutingRequest, 0)
EndUnit()