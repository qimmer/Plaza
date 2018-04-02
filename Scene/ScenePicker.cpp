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
#include <algorithm>
#include <Rendering/UniformState.h>
#include <Foundation/Stream.h>
#include <Rendering/Uniform.h>
#include <Core/Types.h>
#include <Rendering/ShaderVariation.h>

struct ScenePicker {
    ScenePicker() : ScenePickerFov(0.5f) {}

    Entity CommandList, RenderTarget, RenderTexture, BlitTexture, DepthTexture;
    v2f ScenePickerViewportLocation;
    u8 ScenePickerLayers;
    float ScenePickerFov;
    Entity ScenePickerPickedEntity;
};

DefineComponent(ScenePicker)
    Dependency(Camera)
    DefinePropertyReactive(v2f, ScenePickerViewportLocation)
    DefinePropertyEnum(u8, ScenePickerLayers, Layer)
    DefinePropertyReactive(Entity, ScenePickerPickedEntity)
EndComponent()

DefineComponentPropertyReactive(ScenePicker, v2f, ScenePickerViewportLocation)
DefineComponentPropertyReactive(ScenePicker, u8, ScenePickerLayers)
DefineComponentPropertyReactive(ScenePicker, Entity, ScenePickerPickedEntity)

Entity PickingRoot, PickIdUniform, PickShaderVariation;

static void OnReadback(Entity sourceTexture, Entity blitTexture, const u8* data) {
    Entity pickedEntity = 0;
    auto scenePicker = GetParent(sourceTexture);
    if(HasScenePicker(scenePicker)) {
        auto entityIndex = *((const u32*)data);
        if(entityIndex > 0) {
            auto entity = GetEntityFromIndex(entityIndex);
            if(IsEntityValid(entity)) {
                pickedEntity = entity;
            }
        }

        SetScenePickerPickedEntity(scenePicker, pickedEntity);
    }
}

static void UpdateBatches(Entity scenePicker) {
    auto data = GetScenePicker(scenePicker);

    // First, prepare command list
    auto commandList = data->CommandList;

    auto size = GetRenderTargetSize(data->RenderTarget);

    SetCommandListViewport(commandList, {0, 0, size.x, size.y});
    SetCommandListClearColor(commandList, {255, 255, 255, 255});
    SetCommandListRenderTarget(commandList, data->RenderTarget);
    SetCommandListClearDepth(commandList, 1.0f);
    SetCommandListClearTargets(commandList, ClearTarget_Color | ClearTarget_Depth);
    SetCommandListLayer(commandList, GetCameraLayer(scenePicker) + 1);

    TextureReadback(data->RenderTexture, OnReadback);

    m4x4f pickViewMatrix, pickProjectionMatrix, lookMatrix;
    auto pickOrigin = GetCameraPickRayPoint(scenePicker, data->ScenePickerViewportLocation, 0.0f);
    auto pickEnd = GetCameraPickRayPoint(scenePicker, data->ScenePickerViewportLocation, 1.0f);
    v4f pickUp = {0.0f, 1.0f, 0.0f, 0.0f};
    v4f pickDir = {
        pickEnd.x - pickOrigin.x,
        pickEnd.y - pickOrigin.y,
        pickEnd.z - pickOrigin.z,
        0.0f
    };
    glm_vec_normalize(&pickDir.x);
    v4f pickRight = {0.0f, 0.0f, 0.0f, 0.0f};
    glm_vec_cross(&pickDir.x, &pickUp.x, &pickRight.x);

    m4x4f pickCameraWorld = {
        {-pickRight.x, -pickRight.y, -pickRight.z, 0.0f},
        pickUp,
        pickDir,
        {pickOrigin.x, pickOrigin.y, pickOrigin.z, 1.0f}
    };

    glm_mat4_inv((vec4*)&pickCameraWorld, (vec4*) &pickViewMatrix);

    glm_perspective(glm_rad(data->ScenePickerFov), 1.0f, 0.1f, 100.0f, (vec4*)&pickProjectionMatrix);
    vec3 scale = {1.0f, 1.0f, -1.0f};
    glm_scale((vec4*)&pickProjectionMatrix.x, scale);

    SetCommandListViewMatrix(commandList, pickViewMatrix);
    SetCommandListProjectionMatrix(commandList, pickProjectionMatrix);

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
            auto state = CreateUniformState(batch, "IdState");
            SetUniformStateUniform(state, PickIdUniform);
        }

        auto material = GetMeshInstanceMaterial(meshInstance);

        SetBatchMaterial(batch, material);
        SetBatchShaderVariation(batch, PickShaderVariation);
        SetBatchMesh(batch, GetMeshInstanceMesh(meshInstance));
        SetBatchWorldMatrix(batch, GetGlobalTransform(meshInstance));

        auto meshInstanceIndex = GetHandleIndex(meshInstance);
        auto *idComponents = (u8*)&meshInstanceIndex;
        SetUniformStateVec4(GetFirstChild(batch), {
            (float)idComponents[0] / 255.0f,
            (float)idComponents[1] / 255.0f,
            (float)idComponents[2] / 255.0f,
            (float)idComponents[3] / 255.0f
        });

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
    SetTextureFormat(data->RenderTexture, TextureFormat_BGRA8);
    SetTextureFlag(data->RenderTexture, TextureFlag_MIN_POINT | TextureFlag_MAG_POINT | TextureFlag_MIP_POINT | TextureFlag_U_CLAMP | TextureFlag_V_CLAMP | TextureFlag_RT);

    data->DepthTexture = CreateTexture2D(entity, "ScenePicker_DepthTexture");
    SetTextureFormat(data->DepthTexture, TextureFormat_D24S8);
    SetTextureFlag(data->DepthTexture, TextureFlag_MIN_POINT | TextureFlag_MAG_POINT | TextureFlag_MIP_POINT | TextureFlag_U_CLAMP | TextureFlag_V_CLAMP | TextureFlag_RT);

    data->BlitTexture = CreateTexture2D(entity, "ScenePicker_BlitTexture");
    SetTextureFormat(data->BlitTexture, TextureFormat_BGRA8);
    SetTextureFlag(data->BlitTexture, TextureFlag_MIN_POINT | TextureFlag_MAG_POINT | TextureFlag_MIP_POINT | TextureFlag_U_CLAMP | TextureFlag_V_CLAMP | TextureFlag_BLIT_DST | TextureFlag_READ_BACK);
    SetTextureSize2D(data->BlitTexture, {1, 1});

    SetRenderTargetTexture0(data->RenderTarget, data->RenderTexture);
    SetRenderTargetTexture1(data->RenderTarget, data->DepthTexture);
    SetRenderTargetSize(data->RenderTarget, {1, 1});

    SetTextureReadbackTarget(data->RenderTexture, data->BlitTexture);
}

static void OnScenePickerRemoved(Entity entity) {
    DestroyEntity(GetScenePicker(entity)->CommandList);
    DestroyEntity(GetScenePicker(entity)->RenderTarget);
    DestroyEntity(GetScenePicker(entity)->RenderTexture);
    DestroyEntity(GetScenePicker(entity)->BlitTexture);
}

static void InitializePickRoot(Entity entity) {
    PickIdUniform = CreateUniform(entity, "PickIdUniform");
    SetUniformName(PickIdUniform, "pickId");
    SetUniformType(PickIdUniform, TypeOf_v4f());

    PickShaderVariation = CreateShaderVariation(entity, "PickShaderVariation");
    SetShaderVariationDefines(PickShaderVariation, "PICK");
}

DefineService(ScenePicker)
    Subscribe(ScenePickerAdded, OnScenePickerAdded)
    Subscribe(ScenePickerRemoved, OnScenePickerRemoved)
    Subscribe(AppUpdate, OnAppUpdate)
    ServiceEntity(PickingRoot, InitializePickRoot)
EndService()
