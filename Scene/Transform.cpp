//
// Created by Kim Johannsen on 27/01/2018.
//

#include <cglm/cglm.h>
#include <Foundation/Invalidation.h>

#include "Transform.h"

struct Transform {
    m4x4f TransformGlobalMatrix, TransformLocalMatrix;
    Entity TransformParent;
    bool GlobalInvalidated;
};

LocalFunction(OnTransformValidation, void, Entity component) {
    for_entity(entity, transformData, Transform, {
        if(!IsDirty(entity)) continue;

        auto local = transformData->TransformLocalMatrix;
        auto parent = GetTransformParent(entity);

        m4x4f global = local;

        if(HasComponent(parent, ComponentOf_Transform()) > 0) {
            auto parentGlobalMatrix = GetTransformGlobalMatrix(parent);
            glm_mat4_mul((vec4*)&parentGlobalMatrix, (vec4*)&local, (vec4*)&global);

        }

        SetTransformGlobalMatrix(entity, global);
    });
}

LocalFunction(OnAdded, void, Entity component, Entity entity) {
    SetTransformLocalMatrix(entity, m4x4f_Identity);
    SetTransformGlobalMatrix(entity, m4x4f_Identity);
}

BeginUnit(Transform)
    BeginComponent(Transform)
        RegisterProperty(m4x4f, TransformLocalMatrix)
        RegisterPropertyReadOnly(m4x4f, TransformGlobalMatrix)
        RegisterReferenceProperty(Transform, TransformParent)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TransformLocalMatrix()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TransformParent()), Invalidate, 0)
    RegisterSubscription(EventOf_EntityComponentAdded(), OnAdded, ComponentOf_Transform())
    RegisterSubscription(EventOf_Validate(), OnTransformValidation, ComponentOf_Transform())
EndUnit()
