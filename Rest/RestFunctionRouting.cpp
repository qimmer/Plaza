//
// Created by Kim on 06-08-2018.
//

#include "RestFunctionRouting.h"

struct RestFunctionRouting {
    Entity RestFunctionRoutingFunction;
};

static Entity handleGet(StringRef path, Entity request, Entity response) {
    auto responseStream = GetHttpResponseContentStream(response);
    SetStreamPath(responseStream, path);
    if(!StreamOpen(responseStream, StreamMode_Read)) {
        RemoveComponent(responseStream, ComponentOf_Stream());
        return FindResponseCode(404);
    }

    StreamClose(responseStream);

    return FindResponseCode(200);
}

LocalFunction(OnRestRoutingRequest, void, Entity routing, Entity request, Entity response) {
    auto data = GetRestFunctionRoutingData(routing);

    if(data) {
        char completeRoute[1024];
        auto requestUrl = GetHttpRequestUrl(request);
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing));
        snprintf(completeRoute, 1024, "%s/%s", data->RestFunctionRoutingRoot, relativeUrl);

        // 501 if handlers do not set any response code
        auto responseCode = FindResponseCode(501);

        auto method = GetHttpRequestMethod(request);
        if(strcmp(method, "GET") == 0) responseCode = handleGet(completeRoute, request, response);
        else if(strcmp(method, "PUT") == 0) responseCode =handlePut(completeRoute, request, response);
        else if(strcmp(method, "POST") == 0) responseCode =handlePost(completeRoute, request, response);
        else if(strcmp(method, "DELETE") == 0) responseCode =handleDelete(completeRoute, request, response);
        else responseCode = FindResponseCode(405);

        SetHttpResponseCode(response, responseCode);
    }
}

BeginUnit(RestFunctionRouting)
    BeginComponent(RestFunctionRouting)
        RegisterBase(RestRouting)
        RegisterProperty(StringRef, RestFunctionRoutingRoot)
        RegisterProperty(StringRef, RestFunctionRoutingDefaultFile)
    EndComponent()

    RegisterSubscription(RestRoutingRequest, OnRestRoutingRequest, 0)
EndUnit()