//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_RESTRESOURCEROUTING_H
#define PLAZA_RESTRESOURCEROUTING_H

#include <Core/NativeUtils.h>

Unit(RestResourceRouting)
    Component(RestResourceRouting)
        Property(StringRef, RestResourceRoutingRoot)
        Property(StringRef, RestResourceRoutingDefaultFile)

    Function(ResourceRouteGet, Entity, Entity route, Entity request, Entity response, StringRef path)
    Function(ResourceRoutePut, Entity, Entity route, Entity request, Entity response, StringRef path)
    Function(ResourceRoutePost, Entity, Entity route, Entity request, Entity response, StringRef path)
    Function(ResourceRouteDelete, Entity, Entity route, Entity request, Entity response, StringRef path)

#endif //PLAZA_RESTRESOURCEROUTING_H
