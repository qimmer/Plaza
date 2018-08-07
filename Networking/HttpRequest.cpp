//
// Created by Kim on 15/07/2018.
//

#include <Foundation/Stream.h>
#include "HttpRequest.h"

struct HttpHeaderType {
    char HttpHeaderTypeIdentifier[256];
};

struct HttpResponseCode {
    char HttpResponseCodeMessage[64];
    u16 HttpResponseCodeNumber;
};

struct HttpHeader {
    Entity HttpHeaderType;
    char HttpHeaderValue[1024];
};

struct HttpRequest {
    char HttpRequestMethod[16];
    char HttpRequestVersion[16];
    char HttpRequestUrl[512];
    Vector(HttpRequestHeaders, Entity, 32);
    Entity HttpRequestContentStream;
};

struct HttpResponse {
    Entity HttpResponseCode;
    Vector(HttpResponseHeaders, Entity, 32);
    Entity HttpResponseContentStream;
};

API_EXPORT Entity FindHeaderType(const char *identifier) {
    for_entity(headerType, data, HttpHeaderType) {
        auto headerIdentifier = GetHttpHeaderTypeIdentifier(headerType);
        if(stricmp(headerIdentifier, identifier) == 0) {
            return headerType;
        }
    }

    return 0;
}

API_EXPORT Entity FindResponseCode(u16 responseCodeNumber) {
    for_entity(responseCode, data, HttpResponseCode) {
        if(GetHttpResponseCodeNumber(responseCode) == responseCodeNumber) {
            return responseCode;
        }
    }

    return 0;
}

BeginUnit(HttpRequest)
    BeginComponent(HttpHeaderType)
        RegisterProperty(StringRef, HttpHeaderTypeIdentifier)
    EndComponent()

    BeginComponent(HttpResponseCode)
        RegisterProperty(StringRef, HttpResponseCodeMessage)
        RegisterProperty(u16, HttpResponseCodeNumber)
    EndComponent()

    BeginComponent(HttpHeader)
        RegisterReferenceProperty(HttpHeaderType, HttpHeaderType)
        RegisterProperty(StringRef, HttpHeaderValue)
    EndComponent()

    BeginComponent(HttpRequest)
        RegisterProperty(StringRef, HttpRequestMethod)
        RegisterProperty(StringRef, HttpRequestUrl)
        RegisterProperty(StringRef, HttpRequestVersion)
        RegisterArrayProperty(HttpHeader, HttpRequestHeaders)
        RegisterChildProperty(Stream, HttpRequestContentStream)
    EndComponent()

    BeginComponent(HttpResponse)
        RegisterReferenceProperty(HttpResponseCode, HttpResponseCode)
        RegisterArrayProperty(HttpHeader, HttpResponseHeaders)
        RegisterChildProperty(Stream, HttpResponseContentStream)
    EndComponent()
EndUnit()