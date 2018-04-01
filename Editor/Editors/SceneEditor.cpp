//
// Created by Kim Johannsen on 19-03-2018.
//

#include <ImGui/ImGuiRenderer.h>
#include "SceneEditor.h"
#include <imgui/imgui.h>
#include <Scene/Camera.h>
#include <Scene/MeshInstance.h>
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/Material.h>
#include <Rendering/UniformState.h>
#include <Rendering/Uniform.h>
#include <Scene3D/Camera3D.h>
#include <Scene/SimpleSceneRenderer.h>
#include <Scene/Scene.h>
#include <Scene3D/Transform3D.h>
#include <Scene/SceneNode.h>
#include <Core/String.h>

Entity PlaneMesh, PlaneTexture, PlaneMaterial;
Entity CurrentCamera = 0;

struct SceneEditor {
    Entity SceneEditorScene, Camera, Plane, RenderTarget, ColorTexture, DepthTexture, RenderTargetMaterial;
};

DefineComponent(SceneEditor)
    DefinePropertyReactive(Entity, SceneEditorScene)
    DefineExtensionMethod(Scene, void, EditScene, Entity entity)
EndComponent()

DefineComponentPropertyReactive(SceneEditor, Entity, SceneEditorScene)

void EditScene(Entity scene) {
    char editorPath[PATH_MAX];
    snprintf(editorPath, PATH_MAX, "/.editor/SceneEditor/%llu", scene);

    auto editor = CreateEntityFromPath(editorPath);
    SetSceneEditorScene(editor, scene);
}

Entity GetCurrentSceneEditorCamera() {
    return CurrentCamera;
}


static void Draw(Entity context) {
    CurrentCamera = 0;

    for_entity(sceneEditor, SceneEditor) {
        auto data = GetSceneEditor(sceneEditor);
        auto scene = GetSceneEditorScene(sceneEditor);

        bool show = true;
        char title[PATH_MAX];
        snprintf(title, PATH_MAX, "Scene Editor - %s", GetEntityPath(scene));

        if(ImGui::Begin(title, &show)) {
            auto size = ImGui::GetContentRegionAvail();
            auto renderTargetSize = GetRenderTargetSize(data->RenderTarget);
            if(size.x != renderTargetSize.x || size.y != renderTargetSize.y) {
                SetRenderTargetSize(data->RenderTarget, {(s32)size.x, (s32)size.y});
                SetCamera3DAspectRatio(data->Camera, size.x / size.y);
            }

            if(ImGui::BeginChild("frame", size, false, ImGuiWindowFlags_NoMove)) {
                ImGui::PushAllowKeyboardFocus(true);
                if(ImGui::IsWindowHovered() && (ImGui::IsMouseDown(0) || ImGui::IsMouseDown(1))) {
                    ImGui::SetKeyboardFocusHere(0);
                    ImGui::SetWindowFocus();
                }
                ImGui::Image((ImTextureID)(size_t)GetHandleIndex(data->RenderTargetMaterial), size);
                ImGui::PopAllowKeyboardFocus();
                if(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
                    CurrentCamera = data->Camera;
                }
            }
            ImGui::EndChild();

        }

        ImGui::End();


    }
}

static void OnAdded(Entity sceneEditor) {
    auto data = GetSceneEditor(sceneEditor);

    data->Plane = CreateMeshInstance(sceneEditor, "Plane");

    data->ColorTexture = CreateTexture2D(sceneEditor, "ColorTexture");
    SetTextureFlag(data->ColorTexture, TextureFlag_RT | TextureFlag_MAG_POINT | TextureFlag_MIN_POINT);
    SetTextureFormat(data->ColorTexture, TextureFormat_RGBA8);

    data->DepthTexture = CreateTexture2D(sceneEditor, "DepthTexture");
    SetTextureFlag(data->DepthTexture, TextureFlag_RT | TextureFlag_MAG_POINT | TextureFlag_MIN_POINT);
    SetTextureFormat(data->DepthTexture, TextureFormat_D32);

    data->RenderTarget = CreateOffscreenRenderTarget(sceneEditor, "RenderTarget");
    SetRenderTargetTexture0(data->RenderTarget, data->ColorTexture);
    SetRenderTargetTexture1(data->RenderTarget, data->DepthTexture);
    SetRenderTargetSize(data->RenderTarget, {400, 300});

    data->RenderTargetMaterial = CreateMaterial(sceneEditor, "RenderTargetMaterial");
    SetMaterialVertexShader(data->RenderTargetMaterial, GetImGuiVertexShader());
    SetMaterialPixelShader(data->RenderTargetMaterial, GetImGuiPixelShader());
    SetMaterialDepthTest(data->RenderTargetMaterial, RenderState_STATE_DEPTH_TEST_NONE);
    SetMaterialBlendMode(data->RenderTargetMaterial, RenderState_STATE_BLEND_NORMAL);

    auto textureState = CreateUniformState(data->RenderTargetMaterial, "TextureState");
    SetUniformStateUniform(textureState, GetImGuiTextureUniform());
    SetUniformStateTexture(textureState, data->ColorTexture);

    data->Camera = CreateCamera3D(sceneEditor, "Camera");
    SetCameraRenderTarget(data->Camera, data->RenderTarget);
    SetCameraClear(data->Camera, true);
    SetCameraNearClip(data->Camera, 0.1f);
    SetCameraFarClip(data->Camera, 1000.0f);
    SetCameraClearColor(data->Camera, {0, 0, 0, 255});
    SetPosition3D(data->Camera, {5,5,-5});
    SetRotationEuler3D(data->Camera, {45, -45, 0});

    AddSimpleSceneRenderer(data->Camera);

}

static void OnRemoved(Entity sceneEditor) {
    auto data = GetSceneEditor(sceneEditor);
    DestroyEntity(data->Camera);
    DestroyEntity(data->Plane);
    DestroyEntity(data->ColorTexture);
    DestroyEntity(data->DepthTexture);
    DestroyEntity(data->RenderTarget);
    DestroyEntity(data->RenderTargetMaterial);

}

static void OnSceneEditorSceneChanged(Entity sceneEditor, Entity oldScene, Entity newScene) {
    auto data = GetSceneEditor(sceneEditor);

    SetScene(data->Camera, newScene);
    SetScene(data->Plane, newScene);
}

DefineService(SceneEditor)
        Subscribe(ImGuiDraw, Draw)
        Subscribe(SceneEditorAdded, OnAdded)
        Subscribe(SceneEditorRemoved, OnRemoved)
        Subscribe(SceneEditorSceneChanged, OnSceneEditorSceneChanged)
EndService()
