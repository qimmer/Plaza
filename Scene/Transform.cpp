//
// Created by Kim Johannsen on 27/01/2018.
//

#include <cglm/cglm.h>

#include "Transform.h"

struct Transform {
    m4x4f TransformGlobalMatrix, TransformLocalMatrix;
    Entity TransformParent;
};

static void UpdateGlobal(Entity entity) {
    auto local = GetTransformLocalMatrix(entity);
    auto parent = GetTransformParent(entity);

    m4x4f global = local;

    if(HasComponent(parent, ComponentOf_Transform()) > 0) {
        auto parentGlobalMatrix = GetTransformGlobalMatrix(parent);
        glm_mat4_mul((vec4*)&parentGlobalMatrix, (vec4*)&local, (vec4*)&global);

    }

    SetTransformGlobalMatrix(entity, global);

    for_entity(child, data, Transform) {
        if(data->TransformParent == entity) {
            UpdateGlobal(child);
        }
    }
}

LocalFunction(OnLocalChanged, void, Entity entity, m4x4f oldMatrix, m4x4f newMatrix) {
    UpdateGlobal(entity);
}

LocalFunction(OnParentChanged, void, Entity child, Entity oldParent, Entity newParent) {
    if(HasComponent(child, ComponentOf_Transform())) {
        UpdateGlobal(child);
    }
}

BeginUnit(Transform)
    BeginComponent(Transform)
        RegisterProperty(m4x4f, TransformLocalMatrix)
        RegisterPropertyReadOnly(m4x4f, TransformGlobalMatrix)
    EndComponent()

    RegisterSubscription(TransformLocalMatrixChanged, OnLocalChanged, 0)
    RegisterSubscription(TransformParentChanged, OnParentChanged, 0)
EndUnit()
