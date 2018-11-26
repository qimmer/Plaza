//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_TRANSFORM_H
#define PLAZA_TRANSFORM_H

#include <Core/NativeUtils.h>

struct Transform {
    m4x4f TransformGlobalMatrix, TransformLocalMatrix;
    s32 TransformHierarchyLevel;

    union {
        v3f Position3D;
        struct {
            v2f Position2D;
            float Distance2D;
        };
    };

    union {
        v3f RotationEuler3D;
        struct {
            float _padding[2];
            float Rotation2D;
        };
    };

    v4f RotationQuat3D;

    union {
        v3f Scale3D;
        v2f Scale2D;
    };
};

Unit(Transform)
    Component(Transform)
        Property(m4x4f, TransformGlobalMatrix)
        Property(m4x4f, TransformLocalMatrix)
        Property(s32, TransformHierarchyLevel)
        Property(v3f, Position3D)
        Property(v3f, RotationEuler3D)
        Property(v4f, RotationQuat3D)
        Property(v3f, Scale3D)
        Property(v2f, Position2D)
        Property(float, Rotation2D)
        Property(v2f, Scale2D)
        Property(float, Distance2D)

    Function(TransformPoint, v3f, Entity sourceSpace, Entity destinationSpace, v3f sourcePoint)
    Function(TransformNormal, v3f, Entity sourceSpace, Entity destinationSpace, v3f sourceNormal)

void Move3D(Entity transform, v3f direction, bool relativeToRotation);
void LookAt(Entity transform, v3f origin, v3f direction, v3f up = {0.0f, 1.0f, 0.0f});

#endif //PLAZA_TRANSFORM_H
