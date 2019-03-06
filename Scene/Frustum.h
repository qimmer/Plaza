//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_FRUSTUM_H
#define PLAZA_FRUSTUM_H

#include <Core/NativeUtils.h>

struct Frustum {
	v4f FrustumViewMatrix[4];
	v4f FrustumInvViewProjectionMatrix[4];
	v4f FrustumProjectionMatrix[4];
    float FrustumNearClip, FrustumFarClip;
};

Unit(Frustum)
    Component(Frustum)
        Property(float, FrustumNearClip)
        Property(float, FrustumFarClip)

	Declare(AppLoop, FrustumTransformUpdate)
#define AppLoopOrder_FrustumTransformUpdate (AppLoopOrder_TransformUpdate + 0.1f)

#endif //PLAZA_CAMERA_H
