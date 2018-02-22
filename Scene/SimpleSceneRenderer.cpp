//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Camera.h>
#include <Rendering/CommandList.h>
#include <Scene/MeshInstance.h>
#include <Rendering/Batch.h>
#include <Core/Hierarchy.h>
#include <Foundation/Invalidation.h>
#include <Rendering/Mesh.h>
#include <Scene/SceneNode.h>
#include <Scene/Transform.h>
#include <Foundation/AppLoop.h>
#include <cglm/cglm.h>
#include <Rendering/Context.h>
#include <Core/String.h>
#include "SimpleSceneRenderer.h"


    struct SimpleSceneRenderer {
        Entity CommandList;
    };

    DefineComponent(SimpleSceneRenderer)
        Dependency(Camera)
    EndComponent()

    DefineService(SimpleSceneRenderer)
    EndService()

    static void UpdateBatches(Entity sceneRenderer) {
        auto data = GetSimpleSceneRenderer(sceneRenderer);

        // First, prepare command list
        auto commandList = data->CommandList;
        auto renderTarget = GetCameraRenderTarget(sceneRenderer);
        auto viewportMultiplier = GetCameraViewport(sceneRenderer);
        v2i size;

        if(IsEntityValid(renderTarget)) {
            if(HasContext(renderTarget)) {
                size = GetContextSize(renderTarget);
            }
        }

        v4i viewport = {viewportMultiplier.x * size.x,
                        viewportMultiplier.y * size.y,
                        viewportMultiplier.z * size.x,
                        viewportMultiplier.w * size.y};

        SetCommandListViewport(commandList, viewport);
        SetCommandListClearColor(commandList, GetCameraClearColor(sceneRenderer));
        SetCommandListRenderTarget(commandList, GetCameraRenderTarget(sceneRenderer));
        SetCommandListClearDepth(commandList, 0.0f);
        SetCommandListClearTargets(commandList, ClearTarget_Color | ClearTarget_Depth);
        SetCommandListViewMatrix(commandList, GetCameraViewMatrix(sceneRenderer));
        SetCommandListProjectionMatrix(commandList, GetCameraProjectionMatrix(sceneRenderer));

        // Now, update batches
        auto batch = GetFirstChild(data->CommandList);

        for(auto i = 0; i < GetNumMeshInstance(); ++i) {
            auto meshInstance = GetMeshInstanceEntity(i);

            if(GetSceneNodeScene(meshInstance) != GetSceneNodeScene(sceneRenderer)) continue;

            if(IsEntityValid(batch)) {
                batch = GetSibling(batch);
            } else {
                batch = CreateBatch(FormatString("%s/Batch_%i", GetEntityPath(data->CommandList), i));
            }

            SetBatchMaterial(batch, GetMeshInstanceMaterial(meshInstance));
            SetBatchMesh(batch, GetMeshInstanceMesh(meshInstance));
            SetBatchWorldMatrix(batch, GetGlobalTransform(meshInstance));
        }
    }

    static void OnAppUpdate(double deltaTime) {
        for(auto i = 0; i < GetNumSimpleSceneRenderer(); ++i) {
            UpdateBatches(GetSimpleSceneRendererEntity(i));
        }
    }

    static void OnSimpleRendererAdded(Entity entity) {
        GetSimpleSceneRenderer(entity)->CommandList = CreateCommandList(FormatString("%s/CommandList", GetEntityPath(entity)));
    }

    static void OnSimpleRendererRemoved(Entity entity) {
        DestroyEntity(GetSimpleSceneRenderer(entity)->CommandList);
    }

    static bool ServiceStart() {
        SubscribeSimpleSceneRendererAdded(OnSimpleRendererAdded);
        SubscribeSimpleSceneRendererRemoved(OnSimpleRendererRemoved);
        SubscribeAppUpdate(OnAppUpdate);

        AddExtension(TypeOf_Camera(), TypeOf_SimpleSceneRenderer());

        return true;
    }

    static bool ServiceStop() {
        RemoveExtension(TypeOf_Camera(), TypeOf_SimpleSceneRenderer());

        UnsubscribeAppUpdate(OnAppUpdate);
        UnsubscribeSimpleSceneRendererAdded(OnSimpleRendererAdded);
        UnsubscribeSimpleSceneRendererRemoved(OnSimpleRendererRemoved);

        return true;
    }
