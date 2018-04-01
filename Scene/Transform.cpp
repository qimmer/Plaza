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
        GlobalInvalidated = LocalInvalidated = true;
		HierarchyDepth = 0;
    }

    m4x4f GlobalTransform, LocalTransform;
    bool GlobalInvalidated, LocalInvalidated;
    u32 HierarchyDepth;
};

DefineComponent(Transform)
    Dependency(Hierarchy)
    DefinePropertyReactive(m4x4f, LocalTransform)
    DefinePropertyReactive(m4x4f, GlobalTransform)
EndComponent()

DefineComponentPropertyReactive(Transform, m4x4f, GlobalTransform)
DefineComponentPropertyReactive(Transform, m4x4f, LocalTransform)

static s32 InvalidationDepth = -1;

static void UpdateTransforms() {
    // Update global transforms according to new local transforms
    for(auto depth = 0; depth <= InvalidationDepth; depth++) {
        for(auto i = 0; i < GetNumTransform(); ++i) {
            auto data = GetTransformByIndex(i);
            auto entity = GetTransformEntity(i);
            if(!data->GlobalInvalidated || data->HierarchyDepth != depth) continue;

            auto oldTransform = data->GlobalTransform;
            data->LocalInvalidated = data->GlobalInvalidated = false;
            auto parent = GetParent(entity);
            if(depth > 0) {
                auto parentGlobalMatrix = GetGlobalTransform(parent);
                glm_mat4_mul((vec4*)&parentGlobalMatrix, (vec4*)&data->LocalTransform, (vec4*)&data->GlobalTransform);
            } else {
                data->GlobalTransform = data->LocalTransform;
            }

            FireEvent(GlobalTransformChanged, entity, oldTransform, data->GlobalTransform);
        }
    }

    // Update local transforms according to new global transforms
    for(auto depth = 0; depth <= InvalidationDepth; depth++) {
        for(auto i = 0; i < GetNumTransform(); ++i) {
            auto data = GetTransformByIndex(i);
            auto entity = GetTransformEntity(i);

            if(!data->LocalInvalidated || data->HierarchyDepth != depth) continue;

            auto oldTransform = data->LocalTransform;
            data->LocalInvalidated = data->GlobalInvalidated = false;

            auto parent = GetParent(entity);
            if(depth > 0) {
                auto parentGlobalMatrix = GetGlobalTransform(parent);
                m4x4f globalParentInv;
                glm_mat4_inv((vec4*)&parentGlobalMatrix, (vec4*)&globalParentInv);
                glm_mat4_mul((vec4*)&data->GlobalTransform, (vec4*)&globalParentInv, (vec4*)&data->LocalTransform);
            } else {
                data->LocalTransform = data->GlobalTransform;
            }

            FireEvent(LocalTransformChanged, entity, oldTransform, data->LocalTransform);
        }
    }

    InvalidationDepth = -1;
}

static void OnGlobalInvalidated(Entity entity, m4x4f oldMatrix, m4x4f newMatrix) {
    auto data = GetTransform(entity);
    data->GlobalInvalidated = true;
    InvalidationDepth = std::max(InvalidationDepth, (s32)data->HierarchyDepth);
}

static void OnLocalInvalidated(Entity entity, m4x4f oldMatrix, m4x4f newMatrix) {
    auto data = GetTransform(entity);
    data->LocalInvalidated = true;
    InvalidationDepth = std::max(InvalidationDepth, (s32)data->HierarchyDepth);
}

static void OnAppUpdate(double deltaTime) {
    UpdateTransforms();
}

DefineService(TransformUpdateService)
        Subscribe(AppUpdate, OnAppUpdate)
        Subscribe(GlobalTransformChanged, OnGlobalInvalidated)
        Subscribe(LocalTransformChanged, OnLocalInvalidated)
EndService()
