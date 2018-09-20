#include "Frustum.h"
#include <Scene/Transform.h>

#include <cglm/cglm.h>

struct Frustum {
    m4x4f FrustumViewMatrix, FrustumInvViewProjectionMatrix, FrustumProjectionMatrix;
    float FrustumNearClip, FrustumFarClip;
};

static void UpdateViewMatrix(Entity frustum) {
    m4x4f viewMat;
    m4x4f globalMat = GetTransformGlobalMatrix(frustum);
    glm_mat4_inv((vec4*)&globalMat.x.x, (vec4*)&viewMat.x.x);
    SetFrustumViewMatrix(frustum, viewMat);
}

LocalFunction(UpdateInvProjectionMatrix, void, Entity frustum, m4x4f oldValue, m4x4f newValue) {
    auto data = GetFrustumData(frustum);
    m4x4f viewProjMat, invViewProjMat;

    glm_mat4_mul((vec4*)&data->FrustumProjectionMatrix, (vec4*)&data->FrustumViewMatrix, (vec4*)&viewProjMat);
    glm_mat4_inv((vec4*)&viewProjMat, (vec4*)&invViewProjMat);

    SetFrustumInvViewProjectionMatrix(frustum, invViewProjMat);
}

LocalFunction(OnGlobalTransformChanged, void, Entity entity, m4x4f oldValue, m4x4f newValue) {
    if(HasComponent(entity, ComponentOf_Frustum())) {
        UpdateViewMatrix(entity);
    }
}
LocalFunction(OnFrustumAdded, void, Entity component, Entity entity) {
    UpdateViewMatrix(entity);
}

BeginUnit(Frustum)
    BeginComponent(Frustum)
        RegisterPropertyReadOnly(m4x4f, FrustumViewMatrix)
        RegisterPropertyReadOnly(m4x4f, FrustumInvViewProjectionMatrix)

        RegisterProperty(m4x4f, FrustumProjectionMatrix)
        RegisterProperty(float, FrustumNearClip)
        RegisterProperty(float, FrustumFarClip)
    EndComponent()

    RegisterSubscription(TransformGlobalMatrixChanged, OnGlobalTransformChanged, 0)
    RegisterSubscription(EntityComponentAdded, OnFrustumAdded, ComponentOf_Frustum())
    RegisterSubscription(FrustumProjectionMatrixChanged, UpdateInvProjectionMatrix, 0)
    RegisterSubscription(FrustumViewMatrixChanged, UpdateInvProjectionMatrix, 0)
EndUnit()
