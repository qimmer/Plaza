//
// Created by Kim on 22-08-2018.
//

#ifndef PLAZA_INVALIDATION_H
#define PLAZA_INVALIDATION_H

#include <Core/NativeUtils.h>

Unit(Invalidation)
    Function(Invalidate, void, Entity entity)
    Function(InvalidateParent, void, Entity child)

    Function(Validate, void, Entity component)
    bool IsDirty(Entity entity);

    Event(Validate, Entity component)

#endif //PLAZA_INVALIDATION_H
