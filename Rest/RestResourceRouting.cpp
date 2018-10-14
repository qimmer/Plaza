//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include <Core/Identification.h>
#include "RestResourceRouting.h"
#include "RestRouting.h"

struct RestResourceRouting {
    StringRef RestResourceRoutingRoot;
    StringRef RestResourceRoutingDefaultFile;
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

static Entity handlePut(StringRef path, Entity request, Entity response) {
    auto requestStream = GetHttpRequestContentStream(request);
    auto responseStream = GetHttpResponseContentStream(response);

    SetStreamPath(responseStream, path);
    if(!StreamOpen(responseStream, StreamMode_Read)) {
        RemoveComponent(responseStream, ComponentOf_Stream());
        return FindResponseCode(404);
    }

    StreamClose(responseStream);
    if(!StreamOpen(responseStream, StreamMode_Write)) {
        return FindResponseCode(403);
    }

    StreamCopy(requestStream, responseStream);
    StreamClose(responseStream);

    RemoveComponent(responseStream, ComponentOf_Stream());

    return FindResponseCode(204);
}

static Entity handlePost(StringRef path, Entity request, Entity response) {
    auto requestStream = GetHttpRequestContentStream(request);
    auto responseStream = GetHttpResponseContentStream(response);

    SetStreamPath(responseStream, path);

    if(StreamOpen(responseStream, StreamMode_Read)) {
        StreamClose(responseStream);
        return FindResponseCode(403);
    }

    if(!StreamOpen(responseStream, StreamMode_Write)) {
        RemoveComponent(responseStream, ComponentOf_Stream());
        return FindResponseCode(403);
    }

    StreamCopy(requestStream, responseStream);
    StreamClose(responseStream);

    RemoveComponent(responseStream, ComponentOf_Stream());

    return FindResponseCode(201);
}

static Entity handleDelete(StringRef path, Entity request, Entity response) {
    auto responseStream = GetHttpResponseContentStream(response);

    SetStreamPath(responseStream, path);
    if(!StreamDelete(responseStream)) {
        RemoveComponent(responseStream, ComponentOf_Stream());
        return FindResponseCode(403);
    }

    RemoveComponent(responseStream, ComponentOf_Stream());

    return FindResponseCode(200);
}

LocalFunction(OnRestRoutingRequest, void, Entity routing, Entity request, Entity response) {
    auto data = GetRestResourceRoutingData(routing);

    if(data) {
        char completeRoute[1024];
        auto requestUrl = GetHttpRequestUrl(request);
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing));
        snprintf(completeRoute, 1024, "%s/%s", data->RestResourceRoutingRoot, relativeUrl);

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

BeginUnit(RestResourceRouting)
    BeginComponent(RestResourceRouting)
        RegisterBase(RestRouting)
        RegisterProperty(StringRef, RestResourceRoutingRoot)
        RegisterProperty(StringRef, RestResourceRoutingDefaultFile)
    EndComponent()

    RegisterSubscription(EventOf_RestRoutingRequest(), OnRestRoutingRequest, 0)
EndUnit()