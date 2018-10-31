//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_TRANSFORM3D_H
#define PLAZA_TRANSFORM3D_H

#include <Core/NativeUtils.h>

Unit(Transform3D)
    Component(Transform3D)
        Property(v3f, Position3D)
        Property(v3f, RotationEuler3D)
        Property(v4f, RotationQuat3D)
        Property(v3f, Scale3D)

void Move3D(Entity transform, v3f direction, bool relativeToRotation);
void LookAt(Entity transform, v3f origin, v3f direction, v3f up = {0.0f, 1.0f, 0.0f});

#endif //PLAZA_TRANSFORM3D_H
