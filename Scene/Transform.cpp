//
// Created by Kim Johannsen on 27/01/2018.
//

#include <Foundation/Invalidation.h>
#include <Foundation/AppLoop.h>
#include "Transform.h"
#include <cglm/cglm.h>
#include <Core/Hierarchy.h>


    struct Transform {
        Transform() {
            GlobalTransform = LocalTransform = m4x4f_Identity;
            GlobalInvalidated = true;
        }

        m4x4f GlobalTransform, LocalTransform;
        bool GlobalInvalidated, LocalInvalidated;
        u32 HierarchyDepth;
    };

    DefineComponent(Transform)
        Dependency(Invalidation)
        Dependency(Hierarchy)
    EndComponent()

    DefineService(TransformUpdateService)
    EndService()

    DefineComponentProperty(Transform, m4x4f, GlobalTransform)
    DefineComponentProperty(Transform, m4x4f, LocalTransform)

    static s32 InvalidationDepth = -1;

    static void UpdateTransforms() {
        // Update global transforms according to new local transforms
        for(auto depth = 0; depth <= InvalidationDepth; depth++) {
            for(auto i = 0; i < GetNumTransform(); ++i) {
                auto data = GetTransformByIndex(i);
                if(!data->GlobalInvalidated || data->HierarchyDepth != depth) continue;

                data->LocalInvalidated = data->GlobalInvalidated = false;
                auto entity = GetTransformEntity(i);
                auto parent = GetParent(entity);
                if(depth > 0) {
                    auto parentGlobalMatrix = GetGlobalTransform(parent);
                    glm_mat4_mul((vec4*)&parentGlobalMatrix, (vec4*)&data->LocalTransform, (vec4*)&data->GlobalTransform);
                } else {
                    data->GlobalTransform = data->LocalTransform;
                }
            }
        }

        // Update local transforms according to new global transforms
        for(auto depth = 0; depth <= InvalidationDepth; depth++) {
            for(auto i = 0; i < GetNumTransform(); ++i) {
                auto data = GetTransformByIndex(i);
                if(!data->LocalInvalidated || data->HierarchyDepth != depth) continue;

                data->LocalInvalidated = data->GlobalInvalidated = false;
                auto entity = GetTransformEntity(i);
                auto parent = GetParent(entity);
                if(depth > 0) {
                    auto parentGlobalMatrix = GetGlobalTransform(parent);
                    m4x4f globalParentInv;
                    glm_mat4_inv((vec4*)&parentGlobalMatrix, (vec4*)&globalParentInv);
                    glm_mat4_mul((vec4*)&data->GlobalTransform, (vec4*)&globalParentInv, (vec4*)&data->LocalTransform);
                } else {
                    data->LocalTransform = data->GlobalTransform;
                }
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

    static bool ServiceStart() {
        SubscribeAppUpdate(OnAppUpdate);
        SubscribeGlobalTransformChanged(OnGlobalInvalidated);
        SubscribeLocalTransformChanged(OnLocalInvalidated);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeAppUpdate(OnAppUpdate);
        UnsubscribeGlobalTransformChanged(OnGlobalInvalidated);
        UnsubscribeLocalTransformChanged(OnLocalInvalidated);
        return true;
    }
