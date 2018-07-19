//
// Created by Kim on 15/07/2018.
//

#include "HttpRequest.h"

struct HttpHeader {
    char HttpHeaderValue[256];
};

struct HttpRequest {
    char HttpRequestMethod[16];
    char HttpRequestVersion[16];
    char HttpRequestUrl[512];
    Entity HttpRequestResponse;
};

struct HttpResponse {
    u16 HttpResponseCode;
};

BeginUnit(HttpRequest)
    BeginComponent(HttpHeader)
        RegisterProperty(StringRef, HttpHeaderValue)
    EndComponent()

    BeginComponent(HttpRequest)
        RegisterProperty(StringRef, HttpRequestMethod)
        RegisterChildProperty(HttpResponse, HttpRequestResponse)
        RegisterProperty(StringRef, HttpRequestUrl)
        RegisterProperty(StringRef, HttpRequestVersion)
    EndComponent()

    BeginComponent(HttpResponse)
        RegisterProperty(u16, HttpResponseCode)
    EndComponent()
EndUnit()