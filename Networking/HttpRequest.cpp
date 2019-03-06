//
// Created by Kim on 15/07/2018.
//

#include <Foundation/Stream.h>
#include "HttpRequest.h"

#ifndef WIN32
#define stricmp strcasecmp
#endif

struct HttpHeaderType {
    StringRef HttpHeaderTypeIdentifier;
};

struct HttpResponseCode {
    StringRef HttpResponseCodeMessage;
    u16 HttpResponseCodeNumber;
};

struct HttpHeader {
    Entity HttpHeaderType;
    StringRef HttpHeaderValue;
};

struct HttpRequest {
    StringRef HttpRequestMethod;
    StringRef HttpRequestVersion;
    StringRef HttpRequestUrl;
    Entity HttpRequestContentStream;
};

struct HttpParameter {
    StringRef HttpParameterName;
    StringRef HttpParameterValue;
};

struct HttpResponse {
    Entity HttpResponseCode;
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
        RegisterArrayProperty(HttpParameter, HttpRequestParameters)
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

    BeginComponent(HttpParameter)
        RegisterProperty(StringRef, HttpParameterName)
        RegisterProperty(StringRef, HttpParameterValue)
    EndComponent()
EndUnit()