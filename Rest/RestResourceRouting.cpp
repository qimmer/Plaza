//
// Created by Kim on 15/07/2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include <Core/Identification.h>
#include "RestResourceRouting.h"

struct RestResourceRouting {
    StringRef RestResourceRoutingRoot;
    StringRef RestResourceRoutingDefaultFile;
};

Entity ResourceRouteGet(Entity route, Entity request, Entity response, StringRef path) {
    auto responseStream = GetHttpResponse(response).HttpResponseContentStream;

    auto streamData = GetStream(responseStream);
    streamData.StreamPath = path;
    SetStream(responseStream, streamData);

    if(!StreamOpen(responseStream, StreamMode_Read)) {
        RemoveComponent(responseStream, ComponentOf_Stream());
        return FindResponseCode(404);
    }

    StreamClose(responseStream);

    return FindResponseCode(200);
}

Entity ResourceRoutePut(Entity route, Entity request, Entity response, StringRef path) {
    auto requestStream = GetHttpRequest(request).HttpRequestContentStream;
    auto responseStream = GetHttpResponse(response).HttpResponseContentStream;

    auto streamData = GetStream(responseStream);
    streamData.StreamPath = path;
    SetStream(responseStream, streamData);

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

Entity ResourceRoutePost(Entity route, Entity request, Entity response, StringRef path) {
    auto requestStream = GetHttpRequest(request).HttpRequestContentStream;
    auto responseStream = GetHttpResponse(response).HttpResponseContentStream;

    auto streamData = GetStream(responseStream);
    streamData.StreamPath = path;
    SetStream(responseStream, streamData);

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

Entity ResourceRouteDelete(Entity route, Entity request, Entity response, StringRef path) {
    auto responseStream = GetHttpResponse(response).HttpResponseContentStream;

    auto streamData = GetStream(responseStream);
    streamData.StreamPath = path;
    SetStream(responseStream, streamData);

    if(!StreamDelete(responseStream)) {
        RemoveComponent(responseStream, ComponentOf_Stream());
        return FindResponseCode(403);
    }

    RemoveComponent(responseStream, ComponentOf_Stream());

    return FindResponseCode(200);
}

BeginUnit(RestResourceRouting)
    BeginComponent(RestResourceRouting)
        RegisterProperty(StringRef, RestResourceRoutingRoot)
        RegisterProperty(StringRef, RestResourceRoutingDefaultFile)
    EndComponent()

    RegisterFunction(ResourceRouteGet)
    RegisterFunction(ResourceRoutePut)
    RegisterFunction(ResourceRoutePost)
    RegisterFunction(ResourceRouteDelete)
EndUnit()