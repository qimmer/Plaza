#include "Frustum.h"
#include <Scene/Transform.h>

#include <cglm/cglm.h>

static void OnWorldTransformChanged(Entity entity, const WorldTransform& oldData, const WorldTransform& newData) {
    if(HasComponent(entity, ComponentOf_Frustum())) {
        auto data = GetFrustum(entity);

        glm_mat4_inv((vec4*)&newData.WorldTransformMatrix[0].x, (vec4*)&data.FrustumViewMatrix[0].x);
        v4f viewProjMat[4];

        glm_mat4_mul((vec4*)&data.FrustumProjectionMatrix, (vec4*)&data.FrustumViewMatrix, (vec4*)&viewProjMat[0].x);
        glm_mat4_inv((vec4*)&viewProjMat[0].x, (vec4*)&data.FrustumInvViewProjectionMatrix[0].x);

        SetFrustum(entity, data);
    }
}

BeginUnit(Frustum)
    BeginComponent(Frustum)
        RegisterBase(Transform)
        RegisterProperty(float, FrustumNearClip)
        RegisterProperty(float, FrustumFarClip)
    EndComponent()

	RegisterDeferredSystem(OnWorldTransformChanged, ComponentOf_WorldTransform(), AppLoopOrder_FrustumTransformUpdate)
EndUnit()
