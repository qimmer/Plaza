//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include <Core/Identification.h>
#include "RestResourceRouting.h"
#include "RestRouting.h"

struct RestResourceRouting {
    char RestResourceRoutingRoot[256];
    char RestResourceRoutingDefaultFile[64];
};

static u16 handleGet(StringRef path, Entity request) {
    auto response = GetHttpRequestResponse(request);
    SetStreamPath(response, path);
    if(!StreamOpen(response, StreamMode_Read)) {
        RemoveComponent(response, ComponentOf_Stream());
        return 404;
    }

    StreamClose(response);

    return 200;
}

static u16 handlePut(StringRef path, Entity request) {
    auto response = GetHttpRequestResponse(request);
    SetStreamPath(response, path);
    if(!StreamOpen(response, StreamMode_Read)) {
        RemoveComponent(response, ComponentOf_Stream());
        return 404;
    }

    StreamClose(response);
    if(!StreamOpen(response, StreamMode_Write)) {
        return 403;
    }

    StreamCopy(request, response);
    StreamClose(response);

    RemoveComponent(response, ComponentOf_Stream());

    return 204;
}

static u16 handlePost(StringRef path, Entity request) {
    auto response = GetHttpRequestResponse(request);
    SetStreamPath(response, path);

    if(StreamOpen(response, StreamMode_Read)) {
        StreamClose(response);
        return 403;
    }

    if(!StreamOpen(response, StreamMode_Write)) {
        RemoveComponent(response, ComponentOf_Stream());
        return 403;
    }

    StreamCopy(request, response);
    StreamClose(response);

    RemoveComponent(response, ComponentOf_Stream());

    return 201;
}

static u16 handleDelete(StringRef path, Entity request) {
    auto response = GetHttpRequestResponse(request);
    SetStreamPath(response, path);
    if(!StreamDelete(response)) {
        RemoveComponent(response, ComponentOf_Stream());
        return 403;
    }

    RemoveComponent(response, ComponentOf_Stream());

    return 200;
}

LocalFunction(OnRestRoutingRequest, void, Entity routing, Entity request) {
    auto data = GetRestResourceRoutingData(routing);

    if(data) {
        char completeRoute[1024];
        auto requestUrl = GetHttpRequestUrl(request);
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing));
        snprintf(completeRoute, 1024, "%s/%s", data->RestResourceRoutingRoot, relativeUrl);

        // 500 if handlers do not set any response code
        auto responseCode = 500;

        auto method = GetHttpRequestMethod(request);
        if(strcmp(method, "GET") == 0) responseCode = handleGet(completeRoute, request);
        else if(strcmp(method, "PUT") == 0) responseCode =handlePut(completeRoute, request);
        else if(strcmp(method, "POST") == 0) responseCode =handlePost(completeRoute, request);
        else if(strcmp(method, "DELETE") == 0) responseCode =handleDelete(completeRoute, request);
        else responseCode = 405;

        SetHttpResponseCode(GetHttpRequestResponse(request), responseCode);
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