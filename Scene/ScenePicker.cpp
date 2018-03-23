//
// Created by Kim Johannsen on 20-03-2018.
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
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/Shader.h>
#include <Rendering/Material.h>
#include <Rendering/Program.h>
#include "ScenePicker.h"

struct ScenePicker {
    Entity CommandList, RenderTarget, RenderTexture;
    bool ScenePickerActive;
    v2f ScenePickerViewportLocation;
    u8 ScenePickerLayers;
};

DefineComponent(ScenePicker)
    Dependency(Camera)
    DefineProperty(bool, ScenePickerActive)
    DefineProperty(v2f, ScenePickerViewportLocation)
    DefinePropertyEnum(u8, ScenePickerLayers, Layer)
EndComponent()

DefineService(ScenePicker)
EndService()

DefineComponentPropertyReactive(ScenePicker, bool, ScenePickerActive)
DefineComponentPropertyReactive(ScenePicker, v2f, ScenePickerViewportLocation)
DefineComponentPropertyReactive(ScenePicker, u8, ScenePickerLayers)

Vector<Entity> PickPrograms;

static void OnProgramUpdated(Entity originalProgram);

static Entity CreatePickProgramForVertexShader(Entity originalProgram) {
    auto programIndex = GetProgramIndex(originalProgram);

    char name[PATH_MAX];
    snprintf(name, PATH_MAX, "/.picking/PickPrograms/%llu", programIndex);
    auto program = CreateEntityFromPath(name);
    OnProgramUpdated(originalProgram);
    return program;
}

static Entity GetPickProgramForVertexShader(Entity originalProgram) {
    auto programIndex = GetProgramIndex(originalProgram);

    if(PickPrograms.size() <= programIndex || !IsEntityValid(PickPrograms[programIndex])) {
        PickPrograms.resize(std::max(PickPrograms.size(), programIndex + 1));
        PickPrograms[programIndex] = CreatePickProgramForVertexShader(originalProgram);
    }

    return PickPrograms[programIndex];
}

static void OnProgramUpdated(Entity originalProgram) {
    auto pickProgram = GetPickProgramForVertexShader(originalProgram);
    auto vs = GetVertexShader(pickProgram);

}


Entity GetPickedEntity(Entity scenePicker) {
    return 0;
}

static void UpdateBatches(Entity scenePicker) {
    auto data = GetScenePicker(scenePicker);

    // First, prepare command list
    auto commandList = data->CommandList;
    v4f viewport = {0, 0, 1, 1};

    bool shouldRender = data->ScenePickerActive;
    SetHidden(commandList, !shouldRender);
    if(!shouldRender) {
        return;
    }

    auto size = GetRenderTargetSize(data->RenderTarget);
    viewport.x *= size.x;
    viewport.y *= size.y;
    viewport.z *= size.x;
    viewport.w *= size.y;

    SetCommandListViewport(commandList, {(int)viewport.x, (int)viewport.y, (int)viewport.z, (int)viewport.w});
    SetCommandListClearColor(commandList, {255, 255, 255, 255});
    SetCommandListRenderTarget(commandList, data->RenderTarget);
    SetCommandListClearDepth(commandList, 1.0f);
    SetCommandListClearTargets(commandList, ClearTarget_Color | ClearTarget_Depth);
    SetCommandListViewMatrix(commandList, GetCameraViewMatrix(scenePicker));
    SetCommandListProjectionMatrix(commandList, GetCameraProjectionMatrix(scenePicker));
    SetCommandListLayer(commandList, GetCameraLayer(scenePicker));

    auto batch = GetFirstChild(data->CommandList);
    auto batchIndex = 0;
    auto scene = GetSceneNodeScene(scenePicker);
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
    for(auto i = 0; i < GetNumScenePicker(); ++i) {
        UpdateBatches(GetScenePickerEntity(i));
    }
}

static void OnScenePickerAdded(Entity entity) {
    auto data = GetScenePicker(entity);

    data->CommandList = CreateCommandList(entity, "ScenePicker_CommandList");
    data->RenderTarget = CreateOffscreenRenderTarget(entity, "ScenePicker_RenderTarget");
    data->RenderTexture = CreateTexture2D(entity, "ScenePicker_RenderTexture");
}

static void OnSimpleRendererRemoved(Entity entity) {
    DestroyEntity(GetScenePicker(entity)->CommandList);
}

static bool ServiceStart() {
    SubscribeScenePickerAdded(OnScenePickerAdded);
    SubscribeScenePickerRemoved(OnSimpleRendererRemoved);
    SubscribeAppUpdate(OnAppUpdate);

    return true;
}

static bool ServiceStop() {
    UnsubscribeAppUpdate(OnAppUpdate);
    UnsubscribeScenePickerAdded(OnScenePickerAdded);
    UnsubscribeScenePickerRemoved(OnSimpleRendererRemoved);

    return true;
}
