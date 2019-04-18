//
// Created by Kim on 20-09-2018.
//

#include "OrthographicFrustum.h"
#include "Frustum.h"

#include <cglm/cglm.h>

struct OrthographicFrustum {
    v2f OrthographicFrustumTopLeft, OrthographicFrustumBottomRight;
};

static void UpdateProjectionMatrix(Entity entity) {
    if(!HasComponent(entity, ComponentOf_OrthographicFrustum())) return;

    auto data = GetOrthographicFrustum(entity);
	auto frustumData = GetFrustum(entity);

    glm_ortho(
        data.OrthographicFrustumTopLeft.x,
        data.OrthographicFrustumBottomRight.x,
        data.OrthographicFrustumBottomRight.y,
        data.OrthographicFrustumTopLeft.y,
        frustumData.FrustumNearClip,
        frustumData.FrustumFarClip,
        (vec4*)&frustumData.FrustumProjectionMatrix[0].x);

    vec3 scale = {1.0f, 1.0f, -1.0f};
    glm_scale((vec4*)&frustumData.FrustumProjectionMatrix[0].x, scale);
}

static void OnFrustumChanged(Entity entity, const Frustum& oldData, const Frustum& newData) {
    if(HasComponent(entity, ComponentOf_OrthographicFrustum()) && (oldData.FrustumNearClip != newData.FrustumNearClip || oldData.FrustumFarClip != newData.FrustumFarClip)) {
        UpdateProjectionMatrix(entity);
    }
}

static void OnOrthographicFrustumChanged(Entity entity, const OrthographicFrustum& oldData, const OrthographicFrustum& newData) {
    UpdateProjectionMatrix(entity);
}

BeginUnit(OrthographicFrustum)
    BeginComponent(OrthographicFrustum)
        RegisterBase(Frustum)
        RegisterProperty(v2f, OrthographicFrustumTopLeft)
        RegisterProperty(v2f, OrthographicFrustumBottomRight)
    EndComponent()

    RegisterSystem(OnFrustumChanged, ComponentOf_Frustum())
    RegisterSystem(OnOrthographicFrustumChanged, ComponentOf_OrthographicFrustum())
EndUnit()