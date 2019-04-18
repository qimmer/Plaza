//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_HTTPREQUEST_H
#define PLAZA_HTTPREQUEST_H

#include <Core/NativeUtils.h>

struct HttpHeaderType {
    ChildArray HttpRequestParameters;
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
    ChildArray HttpRequestHeaders, HttpRequestParameters;
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
    ChildArray HttpResponseHeaders;
    Entity HttpResponseRequest;
    Entity HttpResponseCode;
    Entity HttpResponseContentStream;
};

Unit(HttpRequest)
    Prefab(HttpResponseContentStream)
    Prefab(HttpRequestContentStream)

    Component(HttpHeaderType)
        Property(StringRef, HttpHeaderTypeIdentifier)

    Component(HttpResponseCode)
        Property(StringRef, HttpResponseCodeMessage)
        Property(u16, HttpResponseCodeNumber)

    Component(HttpHeader)
        ReferenceProperty(HttpHeaderType, HttpHeaderType)
        Property(StringRef, HttpHeaderValue)

    Component(HttpParameter)
        Property(StringRef, HttpParameterName)
        Property(StringRef, HttpParameterValue)

    Component(HttpRequest)
        Property(StringRef, HttpRequestMethod)
        Property(StringRef, HttpRequestUrl)
        Property(StringRef, HttpRequestVersion)
        ArrayProperty(HttpHeader, HttpRequestHeaders)
        ArrayProperty(HttpParameter, HttpRequestParameters)
        ChildProperty(Stream, HttpRequestContentStream)

    Component(HttpResponse)
        ReferenceProperty(HttpRequest, HttpResponseRequest)
        ReferenceProperty(HttpResponseType, HttpResponseCode)
        ArrayProperty(HttpHeader, HttpResponseHeaders)
        ChildProperty(Stream, HttpResponseContentStream)

    Function(FindResponseCode, Entity, u16 responseCodeNumber)
    Function(FindHeaderType, Entity, StringRef identifier)




#endif //PLAZA_HTTPREQUEST_H
