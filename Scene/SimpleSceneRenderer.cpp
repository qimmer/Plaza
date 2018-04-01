//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Camera.h>
#include <Rendering/CommandList.h>
#include <Scene/MeshInstance.h>
#include <Rendering/Batch.h>
#include <Core/Hierarchy.h>
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

    SetCommandListViewport(commandList, {(int)viewport.x, (int)viewport.y, (int)viewport.z, (int)viewport.w});
    SetCommandListClearColor(commandList, GetCameraClearColor(sceneRenderer));
    SetCommandListRenderTarget(commandList, GetCameraRenderTarget(sceneRenderer));
    SetCommandListClearDepth(commandList, 1.0f);
    SetCommandListClearTargets(commandList, (GetCameraClear(sceneRenderer) ? ClearTarget_Color : 0) | ClearTarget_Depth);
    SetCommandListViewMatrix(commandList, GetCameraViewMatrix(sceneRenderer));
    SetCommandListProjectionMatrix(commandList, GetCameraProjectionMatrix(sceneRenderer));
    SetCommandListLayer(commandList, GetCameraLayer(sceneRenderer));

    auto batch = GetFirstChild(data->CommandList);
    auto batchIndex = 0;
    auto scene = GetSceneNodeScene(sceneRenderer);
    for(auto i = 0; i < GetNumMeshInstance(); ++i) {
        auto meshInstance = GetMeshInstanceEntity(i);
        auto meshInstanceScene = GetSceneNodeScene(meshInstance);
        if(!meshInstanceScene || meshInstanceScene != scene || (HasVisibility(meshInstance) && GetHidden(meshInstance))) continue;

        if(!IsEntityValid(batch)) {
            char name[PATH_MAX];
            snprintf(name, PATH_MAX, "Batch_%i", batchIndex);
            batch = CreateBatch(data->CommandList, name);
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
    GetSimpleSceneRenderer(entity)->CommandList = CreateCommandList(entity, "SimpleSceneRenderer_CommandList");
}

static void OnSimpleRendererRemoved(Entity entity) {
    DestroyEntity(GetSimpleSceneRenderer(entity)->CommandList);
}

DefineService(SimpleSceneRenderer)
        Subscribe(SimpleSceneRendererAdded, OnSimpleRendererAdded)
        Subscribe(SimpleSceneRendererRemoved, OnSimpleRendererRemoved)
        Subscribe(AppUpdate, OnAppUpdate)
EndService()
