//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Camera.h>
#include <Rendering/CommandList.h>
#include <Scene/MeshInstance.h>
#include <Rendering/Batch.h>
#include <Core/Node.h>
#include <Rendering/Mesh.h>
#include <Scene/SceneNode.h>
#include <Scene/Transform.h>
#include <Foundation/AppLoop.h>
#include <cglm/cglm.h>
#include <Rendering/RenderContext.h>
#include <Core/String.h>
#include <Foundation/Visibility.h>
#include <Rendering/RenderTarget.h>
#include "SimpleSceneRenderer.h"

struct SimpleSceneRenderer {
    Entity CommandList;
};

BeginUnit(SimpleSceneRenderer)
    BeginComponent(SimpleSceneRenderer)
    RegisterBase(Camera)
EndComponent()

static void UpdateBatches(Entity sceneRenderer) {
    auto data = GetSimpleSceneRenderer(sceneRenderer);

    // First, prepare command list
    auto commandList = data->CommandList;
    auto renderTarget = GetCameraRenderTarget(sceneRenderer);
    auto viewport = GetCameraViewport(sceneRenderer);

    bool shouldRender = IsEntityValid(renderTarget) && HasComponent(renderTarget, ComponentOf_RenderTarget());
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
        if(!meshInstanceScene || meshInstanceScene != scene || (HasComponent(meshInstance, ComponentOf_Visibility()) && GetHidden(meshInstance))) continue;

        if(!IsEntityValid(batch)) {
            char name[PathMax];
            snprintf(name, PathMax, "Batch_%i", batchIndex);
            batch = CreateBatch(data->CommandList, name);
        }

        SetBatchMaterial(batch, GetMeshInstanceMaterial(meshInstance));
        SetBatchMesh(batch, GetMeshInstanceMesh(meshInstance));
        SetBatchWorldMatrix(batch, GetGlobalTransform(meshInstance));

        batch = GetSibling(batch);
        batchIndex++;
    }
}

LocalFunction(OnAppUpdate, void, double deltaTime) {
    for(auto i = 0; i < GetNumSimpleSceneRenderer(); ++i) {
        UpdateBatches(GetSimpleSceneRendererEntity(i));
    }
}

LocalFunction(OnSimpleRendererAdded, void, Entity entity) {
    GetSimpleSceneRendererData(entity)->CommandList = CreateCommandList(entity, "SimpleSceneRenderer_CommandList");
}

LocalFunction(OnSimpleRendererRemoved, void, Entity entity) {
    DestroyEntity(GetSimpleSceneRendererData(entity)->CommandList);
}

DefineService(SimpleSceneRenderer)
        RegisterSubscription(SimpleSceneRendererAdded, OnSimpleRendererAdded, 0)
        RegisterSubscription(SimpleSceneRendererRemoved, OnSimpleRendererRemoved, 0)
        RegisterSubscription(AppUpdate, OnAppUpdate, 0)
EndService()
