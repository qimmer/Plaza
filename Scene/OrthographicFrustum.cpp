//
// Created by Kim on 20-09-2018.
//

#include "OrthographicFrustum.h"
#include "Frustum.h"

#include <cglm/cglm.h>

struct OrthographicFrustum {
    v2f OrthographicFrustumTopLeft, OrthographicFrustumBottomRight;
};

LocalFunction(UpdateProjectionMatrix, void, Entity entity) {
    if(!HasComponent(entity, ComponentOf_OrthographicFrustum())) return;

    auto data = GetOrthographicFrustumData(entity);

    m4x4f projection;
    glm_ortho(
        data->OrthographicFrustumTopLeft.x,
        data->OrthographicFrustumBottomRight.x,
        data->OrthographicFrustumTopLeft.y,
        data->OrthographicFrustumBottomRight.y,
        GetFrustumNearClip(entity),
        GetFrustumNearClip(entity),
        (vec4*)&projection);

    vec3 scale = {1.0f, 1.0f, -1.0f};
    glm_scale((vec4*)&projection.x, scale);

    SetFrustumProjectionMatrix(entity, projection);
}

LocalFunction(OnOrthographicFrustumAdded, void, Entity component, Entity entity) {
    UpdateProjectionMatrix(entity);
}

BeginUnit(OrthographicFrustum)
    BeginComponent(OrthographicFrustum)
        RegisterBase(Frustum)
        RegisterProperty(v2f, OrthographicFrustumTopLeft)
        RegisterProperty(v2f, OrthographicFrustumBottomRight)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnOrthographicFrustumAdded, ComponentOf_OrthographicFrustum())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumNearClip()), UpdateProjectionMatrix, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_FrustumFarClip()), UpdateProjectionMatrix, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_OrthographicFrustumTopLeft()), UpdateProjectionMatrix, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_OrthographicFrustumBottomRight()), UpdateProjectionMatrix, 0)
EndUnit()