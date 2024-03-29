//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_FRUSTUM_H
#define PLAZA_FRUSTUM_H

#include <Core/NativeUtils.h>

Unit(Frustum)
    Component(Frustum)
        Property(m4x4f, FrustumViewMatrix)
        Property(m4x4f, FrustumProjectionMatrix)
        Property(m4x4f, FrustumInvViewProjectionMatrix)
        Property(float, FrustumNearClip)
        Property(float, FrustumFarClip)

    Component(PerspectiveFrustum)
        Property(float, PerspectiveFrustumFieldOfView)
        Property(float, PerspectiveFrustumAspectRatio)

    Component(OrthographicFrustum)
        Property(v2f, OrthographicTopLeft)
        Property(v2f, OrthographicBottomRight)

#endif //PLAZA_CAMERA_H
