//
// Created by Kim Johannsen on 27/01/2018.
//

#include <Foundation/AppLoop.h>
#include "Transform.h"
#include <cglm/cglm.h>
#include <Core/Node.h>

struct Transform {
    Transform() {
        GlobalTransform = LocalTransform = m4x4f_Identity;
    }

    m4x4f GlobalTransform, LocalTransform;
};

BeginUnit(Transform)
    BeginComponent(Transform)
    RegisterBase(Node)
    RegisterProperty(m4x4f, LocalTransform)
    RegisterProperty(m4x4f, GlobalTransform)
EndComponent()
EndUnit()
(m4x4f, GlobalTransform)
RegisterProperty(m4x4f, LocalTransform)

static void UpdateGlobal(Entity entity) {
    auto data = GetTransformData(entity);
    auto oldTransform = data->GlobalTransform;
    auto parent = GetParent(entity);

    if(HasComponent(parent, ComponentOf_Transform()) > 0) {
        auto parentGlobalMatrix = GetGlobalTransform(parent);
        glm_mat4_mul((vec4*)&parentGlobalMatrix, (vec4*)&data->LocalTransform, (vec4*)&data->GlobalTransform);
    } else {
        data->GlobalTransform = data->LocalTransform;
    }

    FireEvent(EventOf_GlobalTransformChanged(), entity, oldTransform, data->GlobalTransform);

    for_children(child, entity) {
        if(HasComponent(child, ComponentOf_Transform())) {
            UpdateGlobal(child);
        }
    }
}

LocalFunction(OnLocalInvalidated, void, Entity entity, m4x4f oldMatrix, m4x4f newMatrix) {
    if(memcmp(&oldMatrix, &newMatrix, sizeof(m4x4f)) != 0) {
        UpdateGlobal(entity);
    }
}

LocalFunction(OnParentChanged, void, Entity child, Entity oldParent, Entity newParent) {
    if(HasComponent(child, ComponentOf_Transform())) {
        UpdateGlobal(child);
    }
}

DefineService(TransformUpdateService)
    RegisterSubscription(LocalTransformChanged, OnLocalInvalidated, 0)
    RegisterSubscription(ParentChanged, OnParentChanged, 0)
EndService()
