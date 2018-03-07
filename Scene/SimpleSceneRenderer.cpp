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
#include <Foundation/Visibility.h>
#include <Rendering/RenderTarget.h>
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
        auto viewport = GetCameraViewport(sceneRenderer);

        bool shouldRender = IsEntityValid(renderTarget) && HasRenderTarget(renderTarget);
        SetHidden(commandList, !shouldRender);
        if(!shouldRender) {
            return;
        }

        auto size = GetRenderTargetSize(renderTarget);
        viewport.x *= size.x;
        viewport.y *= size.y;
        viewport.z *= size.x;
        viewport.w *= size.y;

        SetCommandListViewport(commandList, {lroundf(viewport.x), lroundf(viewport.y), lroundf(viewport.z), lroundf(viewport.w)});
        SetCommandListClearColor(commandList, GetCameraClearColor(sceneRenderer));
        SetCommandListRenderTarget(commandList, GetCameraRenderTarget(sceneRenderer));
        SetCommandListClearDepth(commandList, 0.0f);
        SetCommandListClearTargets(commandList, (GetCameraClear(sceneRenderer) ? ClearTarget_Color : 0) | ClearTarget_Depth);
        SetCommandListViewMatrix(commandList, GetCameraViewMatrix(sceneRenderer));
        SetCommandListProjectionMatrix(commandList, GetCameraProjectionMatrix(sceneRenderer));
        SetCommandListLayer(commandList, GetCameraLayer(sceneRenderer));

        auto batch = GetFirstChild(data->CommandList);
        auto batchIndex = 0;
        for(auto i = 0; i < GetNumMeshInstance(); ++i) {
            auto meshInstance = GetMeshInstanceEntity(i);

            if(GetSceneNodeScene(meshInstance) != GetSceneNodeScene(sceneRenderer)) continue;

            if(!IsEntityValid(batch)) {
                batch = CreateBatch(FormatString("%s/Batch_%i", GetEntityPath(data->CommandList), batchIndex));
            }

            SetBatchMaterial(batch, GetMeshInstanceMaterial(meshInstance));
            SetBatchMesh(batch, GetMeshInstanceMesh(meshInstance));
            SetBatchWorldMatrix(batch, GetGlobalTransform(meshInstance));

            batch = GetSibling(batch);
            batchIndex++;
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
