//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_NETWORKINGMODULE_H
#define PLAZA_NETWORKINGMODULE_H

#include <Core/NativeUtils.h>

struct Networking {
    ChildArray HttpHeaderTypes, HttpResponseCodes;
    bool TcpWaitOnNoWork;
};

Module(Networking)

Unit(Networking)
    Component(Networking)
        ArrayProperty(HttpHeaderType, HttpHeaderTypes)
        ArrayProperty(HttpResponseCode, HttpResponseCodes)
        Property(bool, TcpWaitOnNoWork)

#endif //PLAZA_NETWORKINGMODULE_H
