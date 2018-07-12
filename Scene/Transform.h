//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_TRANSFORM_H
#define PLAZA_TRANSFORM_H

#include <Core/NativeUtils.h>

Unit(Transform)
    Component(Transform)

        Property(m4x4f, GlobalTransform)
        Property(m4x4f, LocalTransform)

#endif //PLAZA_TRANSFORM_H
