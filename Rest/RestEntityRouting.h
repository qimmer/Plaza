//
// Created by Kim on 15/07/2018.
//

#ifndef PLAZA_RESTENTITYROUTING_H
#define PLAZA_RESTENTITYROUTING_H

#include <Core/NativeUtils.h>

Unit(RestEntityRouting)
    Component(RestEntityRouting)
        ReferenceProperty(Ownership, RestEntityRoutingRoot)
        Property(u8, RestEntityRoutingDepth)

#endif //PLAZA_RESTENTITYROUTING_H
