//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_RESTENTITYROUTING_H
#define PLAZA_RESTENTITYROUTING_H

#include <Core/NativeUtils.h>

Unit(RestEntityRouting)
    Component(RestEntityRouting)
        ChildProperty(JsonSettings, RestEntityRoutingJsonSettings)

    Function(EntityRouteGet, Entity, Entity route, Entity request, Entity response, StringRef uuid)
    Function(EntityRoutePut, Entity, Entity route, Entity request, Entity response, StringRef uuid)
    Function(EntityRoutePost, Entity, Entity route, Entity request, Entity response, StringRef uuid, StringRef propertyName)
    Function(EntityRouteDelete, Entity, Entity route, Entity request, Entity response, StringRef uuid)

#endif //PLAZA_RESTENTITYROUTING_H
