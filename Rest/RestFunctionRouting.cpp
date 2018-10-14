//
// Created by Kim on 06-08-2018.
//

#include <Networking/HttpRequest.h>
#include <Foundation/Stream.h>
#include "RestFunctionRouting.h"
#include "RestRouting.h"

struct RestFunctionRouting {
    Entity RestFunctionRoutingFunction;
};

LocalFunction(OnRestRoutingRequest, void, Entity routing, Entity request, Entity response) {
    auto data = GetRestFunctionRoutingData(routing);

    if(data) {
        auto requestUrl = GetHttpRequestUrl(request);
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing));

        char arguments[2048];
        strncpy(arguments, relativeUrl, sizeof(arguments));

        // 501 if handlers do not set any response code
        u16 responseCode = 501;

        auto method = GetHttpRequestMethod(request);
        auto responseStream = GetHttpResponseContentStream(response);

        Type argumentTypes[] = {TypeOf_Entity, TypeOf_StringRef};
        const void* argumentData[] = {&responseStream, &relativeUrl};

        CallFunction(data->RestFunctionRoutingFunction, &responseCode, 2, argumentTypes, argumentData);

        SetHttpResponseCode(response, FindResponseCode(responseCode));
    }
}

BeginUnit(RestFunctionRouting)
    BeginComponent(RestFunctionRouting)
        RegisterBase(RestRouting)
        RegisterReferenceProperty(Function, RestFunctionRoutingFunction)
    EndComponent()

    RegisterSubscription(EventOf_RestRoutingRequest(), OnRestRoutingRequest, 0)
EndUnit()