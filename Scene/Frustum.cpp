#include "Frustum.h"
#include <Scene/Transform.h>
#include <Foundation/Invalidation.h>

#include <cglm/cglm.h>

LocalFunction(OnValidateTransforms, void, Entity component) {
    for_entity(frustum, transformData, Frustum) {
        if(!IsDirty(frustum)) continue;

        auto data = GetFrustumData(frustum);
        auto globalMat = GetTransformGlobalMatrix(frustum);

        m4x4f viewMat;
        glm_mat4_inv((vec4*)&globalMat.x.x, (vec4*)&viewMat.x.x);
        SetFrustumViewMatrix(frustum, viewMat);

        m4x4f viewProjMat;
        m4x4f invViewProjMat;

        glm_mat4_mul((vec4*)&data->FrustumProjectionMatrix, (vec4*)&data->FrustumViewMatrix, (vec4*)&viewProjMat);
        glm_mat4_inv((vec4*)&viewProjMat, (vec4*)&invViewProjMat);

        SetFrustumInvViewProjectionMatrix(frustum, invViewProjMat);
    }
}

BeginUnit(Frustum)
    BeginComponent(Frustum)
        RegisterBase(Transform)
        RegisterPropertyReadOnly(m4x4f, FrustumViewMatrix)
        RegisterPropertyReadOnly(m4x4f, FrustumInvViewProjectionMatrix)

        RegisterProperty(m4x4f, FrustumProjectionMatrix)
        RegisterProperty(float, FrustumNearClip)
        RegisterProperty(float, FrustumFarClip)
    EndComponent()

    RegisterSubscription(EventOf_Validate(), OnValidateTransforms, ComponentOf_Transform())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumProjectionMatrix()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumNearClip()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumFarClip()), Invalidate, 0)
EndUnit()
