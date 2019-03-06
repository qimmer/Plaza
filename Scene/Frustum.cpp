#include "Frustum.h"
#include <Scene/Transform.h>
#include <Foundation/Invalidation.h>

#include <cglm/cglm.h>

LocalFunction(OnFrustumTransformUpdate, void, Entity component) {
    for_entity(frustum, transformData, Frustum) {
		auto transformData = GetTransformData(frustum);

        auto data = GetFrustumData(frustum);

        glm_mat4_inv((vec4*)&transformData->TransformGlobalMatrix[0].x, (vec4*)&data->FrustumViewMatrix[0].x);
        v4f viewProjMat[4];

        glm_mat4_mul((vec4*)&data->FrustumProjectionMatrix, (vec4*)&data->FrustumViewMatrix, (vec4*)&viewProjMat[0].x);
        glm_mat4_inv((vec4*)&viewProjMat[0].x, (vec4*)&data->FrustumInvViewProjectionMatrix[0].x);
    }
}

BeginUnit(Frustum)
    BeginComponent(Frustum)
        RegisterBase(Transform)
        RegisterProperty(float, FrustumNearClip)
        RegisterProperty(float, FrustumFarClip)
    EndComponent()

	RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnFrustumTransformUpdate, AppLoopOf_FrustumTransformUpdate())

	SetAppLoopOrder(AppLoopOf_FrustumTransformUpdate(), AppLoopOrder_FrustumTransformUpdate);

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumNearClip()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumFarClip()), Invalidate, 0)
EndUnit()
