//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_TRANSFORM_H
#define PLAZA_TRANSFORM_H

#include <Core/NativeUtils.h>

Unit(Transform)
    Component(Transform)
        Property(m4x4f, TransformGlobalMatrix)
        Property(m4x4f, TransformLocalMatrix)
        ReferenceProperty(Transform, TransformParent)

#endif //PLAZA_TRANSFORM_H
