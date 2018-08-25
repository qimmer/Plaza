//
// Created by Kim on 22-08-2018.
//

#ifndef PLAZA_INVALIDATION_H
#define PLAZA_INVALIDATION_H

#include <Core/NativeUtils.h>

Unit(Invalidation)
    Component(Invalidation)
        Property(bool, DirtyFlag)

Function(Invalidate, void, Entity entity)
Function(Validate, void, Entity entity)

Event(Validate, Entity entity)

#endif //PLAZA_INVALIDATION_H
