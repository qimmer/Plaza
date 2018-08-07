//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_HTTPREQUEST_H
#define PLAZA_HTTPREQUEST_H

#include <Core/NativeUtils.h>

Unit(HttpRequest)
    Component(HttpHeaderType)
        Property(StringRef, HttpHeaderTypeIdentifier)

    Component(HttpResponseCode)
        Property(StringRef, HttpResponseCodeMessage)
        Property(u16, HttpResponseCodeNumber)

    Component(HttpHeader)
        ReferenceProperty(HttpHeaderType, HttpHeaderType)
        Property(StringRef, HttpHeaderValue)

    Component(HttpRequest)
        Property(StringRef, HttpRequestMethod)
        Property(StringRef, HttpRequestUrl)
        Property(StringRef, HttpRequestVersion)
        ArrayProperty(HttpHeader, HttpRequestHeaders)
        ChildProperty(Stream, HttpRequestContentStream)

    Component(HttpResponse)
        ReferenceProperty(HttpResponseType, HttpResponseCode)
        ArrayProperty(HttpHeader, HttpResponseHeaders)
        ChildProperty(Stream, HttpResponseContentStream)

    Function(FindResponseCode, Entity, u16 responseCodeNumber)
    Function(FindHeaderType, Entity, StringRef identifier)




#endif //PLAZA_HTTPREQUEST_H
