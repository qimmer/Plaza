//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_FRUSTUM_H
#define PLAZA_FRUSTUM_H

#include <Core/NativeUtils.h>

struct Frustum {
    m4x4f FrustumViewMatrix, FrustumInvViewProjectionMatrix, FrustumProjectionMatrix;
    float FrustumNearClip, FrustumFarClip;
};

Unit(Frustum)
    Component(Frustum)
        Property(m4x4f, FrustumViewMatrix)
        Property(m4x4f, FrustumProjectionMatrix)
        Property(m4x4f, FrustumInvViewProjectionMatrix)
        Property(float, FrustumNearClip)
        Property(float, FrustumFarClip)

#endif //PLAZA_CAMERA_H
