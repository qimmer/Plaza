//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_NETWORKINGMODULE_H
#define PLAZA_NETWORKINGMODULE_H

#include <Core/NativeUtils.h>

Module(Networking)

Unit(Networking)
    Component(Networking)
        ArrayProperty(HttpHeaderType, HttpHeaderTypes)
        ArrayProperty(HttpResponseCode, HttpResponseCodes)
        Property(bool, TcpWaitOnNoWork)

    Declare(AppLoop, Networking)

#endif //PLAZA_NETWORKINGMODULE_H
