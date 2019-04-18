//
// Created by Kim on 15/07/2018.
//

#include <Foundation/Stream.h>
#include "HttpRequest.h"

#ifndef WIN32
#define stricmp strcasecmp
#endif

API_EXPORT Entity FindHeaderType(StringRef identifier) {
    identifier = Intern(identifier);
    for_entity(headerType, ComponentOf_HttpHeaderType()) {
        auto headerIdentifier = GetHttpHeaderType(headerType).HttpHeaderTypeIdentifier;
        if(headerIdentifier == identifier) {
            return headerType;
        }
    }

    return 0;
}

API_EXPORT Entity FindResponseCode(u16 responseCodeNumber) {
    for_entity(responseCode, ComponentOf_HttpResponseCode()) {
        if(GetHttpResponseCode(responseCode).HttpResponseCodeNumber == responseCodeNumber) {
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
        BeginChildProperty(HttpRequestContentStream)
        EndChildProperty()
    EndComponent()

    BeginComponent(HttpResponse)
        RegisterReferenceProperty(HttpRequest, HttpResponseRequest)
        RegisterReferenceProperty(HttpResponseCode, HttpResponseCode)
        RegisterArrayProperty(HttpHeader, HttpResponseHeaders)
        BeginChildProperty(HttpResponseContentStream)
        EndChildProperty()
    EndComponent()

    BeginComponent(HttpParameter)
        RegisterProperty(StringRef, HttpParameterName)
        RegisterProperty(StringRef, HttpParameterValue)
    EndComponent()
EndUnit()