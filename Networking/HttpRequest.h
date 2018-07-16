//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_HTTPREQUEST_H
#define PLAZA_HTTPREQUEST_H

#include <Core/NativeUtils.h>

Unit(HttpRequest)
    Component(HttpHeader)
        Property(StringRef, HttpHeaderValue)

    Component(HttpRequest)
        Property(StringRef, HttpRequestMethod)
        Property(StringRef, HttpRequestUrl)
        Property(StringRef, HttpRequestVersion)
        Property(Entity, HttpRequestResponse)

    Component(HttpResponse)
        Property(u16, HttpResponseCode)




#endif //PLAZA_HTTPREQUEST_H
