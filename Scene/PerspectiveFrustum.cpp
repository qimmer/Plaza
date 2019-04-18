//
// Created by Kim on 20-09-2018.
//

#include "PerspectiveFrustum.h"
#include "Frustum.h"

#include <cglm/cglm.h>

struct PerspectiveFrustum {
    float PerspectiveFrustumFieldOfView, PerspectiveFrustumAspectRatio;
};

static void UpdateProjectionMatrix(Entity entity) {
    auto data = GetPerspectiveFrustum(entity);
	auto frustumData = GetFrustum(entity);
	auto oldData = frustumData;

    glm_perspective(glm_rad(data.PerspectiveFrustumFieldOfView), data.PerspectiveFrustumAspectRatio, frustumData.FrustumNearClip, frustumData.FrustumFarClip, (vec4*)&frustumData.FrustumProjectionMatrix[0].x);

    vec3 scale = {1.0f, 1.0f, -1.0f};
    glm_scale((vec4*)&frustumData.FrustumProjectionMatrix[0].x, scale);

    if(memcmp(&frustumData.FrustumProjectionMatrix[0], &oldData.FrustumProjectionMatrix[0], sizeof(v4f)*4) != 0) {
        SetFrustum(entity, frustumData);
    }
}

static void OnFrustumChanged(Entity entity, const Frustum& oldData, const Frustum& newData) {
    if(HasComponent(entity, ComponentOf_PerspectiveFrustum()) && (oldData.FrustumNearClip != newData.FrustumNearClip || oldData.FrustumFarClip != newData.FrustumFarClip)) {
        UpdateProjectionMatrix(entity);
    }
}

static void OnPerspectiveFrustumChanged(Entity entity, const PerspectiveFrustum& oldData, const PerspectiveFrustum& newData) {
    UpdateProjectionMatrix(entity);
}

BeginUnit(PerspectiveFrustum)
    BeginComponent(PerspectiveFrustum)
        RegisterBase(Frustum)
        RegisterProperty(float, PerspectiveFrustumFieldOfView)
        RegisterProperty(float, PerspectiveFrustumAspectRatio)
    EndComponent()

    RegisterSystem(OnFrustumChanged, ComponentOf_Frustum())
    RegisterSystem(OnPerspectiveFrustumChanged, ComponentOf_PerspectiveFrustum())
EndUnit()
