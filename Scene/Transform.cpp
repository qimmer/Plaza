//
// Created by Kim Johannsen on 27/01/2018.
//

#include <Foundation/AppLoop.h>
#include "Transform.h"
#include <cglm/cglm.h>
#include <Core/Hierarchy.h>

struct Transform {
    Transform() {
        GlobalTransform = LocalTransform = m4x4f_Identity;
    }

    m4x4f GlobalTransform, LocalTransform;
};

DefineComponent(Transform)
    Dependency(Hierarchy)
    DefinePropertyReactive(m4x4f, LocalTransform)
    DefinePropertyReactive(m4x4f, GlobalTransform)
EndComponent()

DefineComponentPropertyReactive(Transform, m4x4f, GlobalTransform)
DefineComponentPropertyReactive(Transform, m4x4f, LocalTransform)

static void UpdateGlobal(Entity entity) {
    auto data = GetTransform(entity);
    auto oldTransform = data->GlobalTransform;
    auto parent = GetParent(entity);

    if(HasTransform(parent) > 0) {
        auto parentGlobalMatrix = GetGlobalTransform(parent);
        glm_mat4_mul((vec4*)&parentGlobalMatrix, (vec4*)&data->LocalTransform, (vec4*)&data->GlobalTransform);
    } else {
        data->GlobalTransform = data->LocalTransform;
    }

    FireNativeEvent(GlobalTransformChanged, entity, oldTransform, data->GlobalTransform);

    for_children(child, entity) {
        if(HasTransform(child)) {
            UpdateGlobal(child);
        }
    }
}

static void OnLocalInvalidated(Entity entity, m4x4f oldMatrix, m4x4f newMatrix) {
    if(memcmp(&oldMatrix, &newMatrix, sizeof(m4x4f)) != 0) {
        UpdateGlobal(entity);
    }
}

static void OnParentChanged(Entity child, Entity oldParent, Entity newParent) {
    if(HasTransform(child)) {
        UpdateGlobal(child);
    }
}

DefineService(TransformUpdateService)
    Subscribe(LocalTransformChanged, OnLocalInvalidated)
    Subscribe(ParentChanged, OnParentChanged)
EndService()
