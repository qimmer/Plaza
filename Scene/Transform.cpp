//
// Created by Kim Johannsen on 27/01/2018.
//

#include <cglm/cglm.h>
#include <Foundation/Invalidation.h>

#include "Transform.h"

static inline void ValidateTransform(Entity entity, Transform *transformData) {
    auto local = transformData->TransformLocalMatrix;
    auto parent = GetOwner(entity);

    m4x4f global = local;

    if(transformData->TransformHierarchyLevel > 0 && HasComponent(parent, ComponentOf_Transform()) > 0) {
        auto parentGlobalMatrix = GetTransformGlobalMatrix(parent);
        glm_mat4_mul((vec4*)&parentGlobalMatrix, (vec4*)&local, (vec4*)&global);

    }

    SetTransformGlobalMatrix(entity, global);
}

static void CalculateHierarchyLevel(Entity entity) {
    if(HasComponent(entity, ComponentOf_Transform())) {
        auto parent = GetOwner(entity);
        s32 level = 0;
        while(HasComponent(parent, ComponentOf_Transform())) {
            level++;
            parent = GetOwner(parent);
        }

        GetTransformData(entity)->TransformHierarchyLevel = level;
        Invalidate(entity);
    }
}

LocalFunction(OnTransformValidation, void, Entity component) {
    s32 level = 0;
    bool hasAny = false;
    do
    {
        hasAny = false;

        for_entity(entity, transformData, Transform, {
            if(transformData->TransformHierarchyLevel == level) {
                hasAny = true;
            } else {
                continue;
            }

            if(!IsDirty(entity)) continue;

            ValidateTransform(entity, transformData);
        });

        level++;
    } while(hasAny);
}

LocalFunction(OnAdded, void, Entity component, Entity entity) {
    SetTransformLocalMatrix(entity, m4x4f_Identity);
    SetTransformGlobalMatrix(entity, m4x4f_Identity);
    CalculateHierarchyLevel(entity);
    Invalidate(entity);
}

LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    CalculateHierarchyLevel(entity);
}

BeginUnit(Transform)
    BeginComponent(Transform)
        RegisterProperty(m4x4f, TransformLocalMatrix)
        RegisterPropertyReadOnly(m4x4f, TransformGlobalMatrix)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TransformLocalMatrix()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
    RegisterSubscription(EventOf_EntityComponentAdded(), OnAdded, ComponentOf_Transform())
    RegisterSubscription(EventOf_Validate(), OnTransformValidation, ComponentOf_Transform())
EndUnit()
