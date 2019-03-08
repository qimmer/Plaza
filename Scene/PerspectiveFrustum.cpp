//
// Created by Kim on 20-09-2018.
//

#include "PerspectiveFrustum.h"
#include "Frustum.h"

#include <cglm/cglm.h>

struct PerspectiveFrustum {
    float PerspectiveFrustumFieldOfView, PerspectiveFrustumAspectRatio;
};

LocalFunction(UpdateProjectionMatrix, void, Entity entity) {
    if(!HasComponent(entity, ComponentOf_PerspectiveFrustum())) return;

    auto data = GetPerspectiveFrustumData(entity);
	auto frustumData = GetFrustumData(entity);

    glm_perspective(glm_rad(data->PerspectiveFrustumFieldOfView), data->PerspectiveFrustumAspectRatio, GetFrustumNearClip(entity), GetFrustumFarClip(entity), (vec4*)&frustumData->FrustumProjectionMatrix[0].x);

    vec3 scale = {1.0f, 1.0f, -1.0f};
    glm_scale((vec4*)&frustumData->FrustumProjectionMatrix[0].x, scale);
}

LocalFunction(OnPerspectiveFrustumAdded, void, Entity component, Entity entity) {
    UpdateProjectionMatrix(entity);
}

BeginUnit(PerspectiveFrustum)
    BeginComponent(PerspectiveFrustum)
        RegisterBase(Frustum)
        RegisterProperty(float, PerspectiveFrustumFieldOfView)
        RegisterProperty(float, PerspectiveFrustumAspectRatio)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnPerspectiveFrustumAdded, ComponentOf_PerspectiveFrustum())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumNearClip()), UpdateProjectionMatrix, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumFarClip()), UpdateProjectionMatrix, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PerspectiveFrustumFieldOfView()), UpdateProjectionMatrix, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PerspectiveFrustumAspectRatio()), UpdateProjectionMatrix, 0)
EndUnit()
