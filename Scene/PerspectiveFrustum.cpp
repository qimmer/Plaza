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

    m4x4f projection;
    glm_perspective(glm_rad(data->PerspectiveFrustumFieldOfView), data->PerspectiveFrustumAspectRatio, GetFrustumNearClip(entity), GetFrustumFarClip(entity), (vec4*)&projection.x.x);

    vec3 scale = {1.0f, 1.0f, -1.0f};
    glm_scale((vec4*)&projection.x, scale);

    SetFrustumProjectionMatrix(entity, projection);
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

    RegisterSubscription(EntityComponentAdded, OnPerspectiveFrustumAdded, ComponentOf_PerspectiveFrustum())
    RegisterSubscription(FrustumNearClipChanged, UpdateProjectionMatrix, 0)
    RegisterSubscription(FrustumFarClipChanged, UpdateProjectionMatrix, 0)
    RegisterSubscription(PerspectiveFrustumFieldOfViewChanged, UpdateProjectionMatrix, 0)
    RegisterSubscription(PerspectiveFrustumAspectRatioChanged, UpdateProjectionMatrix, 0)
EndUnit()
