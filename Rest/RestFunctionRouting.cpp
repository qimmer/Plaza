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
        auto relativeUrl = requestUrl + strlen(GetRestRoutingRoute(routing)) + 1;

        char arguments[2048];
        strncpy(arguments, relativeUrl, sizeof(arguments));

        // 501 if handlers do not set any response code
        u16 responseCode = 501;

        auto method = GetHttpRequestMethod(request);
        auto responseStream = GetHttpResponseContentStream(response);

        eastl::fixed_vector<Type, 32> argumentTypes;
        eastl::fixed_vector<const void*, 32> argumentData;

        argumentTypes.push_back(TypeOf_Entity);
        argumentData.push_back(&responseStream);

        u32 argumentLength = 0;
        u32 offset = 0;
        while(arguments[offset] != '\0') {
            if(arguments[offset] == '/') {
                argumentTypes.push_back(TypeOf_StringRef);
                argumentData.push_back(&arguments[offset - argumentLength]);
                arguments[offset] = '\0';
                argumentLength = 0;
            } else {
                argumentLength++;
            }

            offset++;
        }

        auto numArguments = GetNumFunctionArguments(data->RestFunctionRoutingFunction);
        if(numArguments != argumentTypes.size()) {
            responseCode = 400;
        } else {
            CallFunction(data->RestFunctionRoutingFunction, &responseCode, argumentData.size(), argumentTypes.data(), argumentData.data());
        }

        SetHttpResponseCode(response, FindResponseCode(responseCode));
    }
}

BeginUnit(RestFunctionRouting)
    BeginComponent(RestFunctionRouting)
        RegisterBase(RestRouting)
        RegisterReferenceProperty(Function, RestFunctionRoutingFunction)
    EndComponent()

    RegisterSubscription(RestRoutingRequest, OnRestRoutingRequest, 0)
EndUnit()