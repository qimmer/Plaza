//
// Created by Kim Johannsen on 22-03-2018.
//

#include "ImGui/imgui/imgui.h"
#include "../../Foundation/Stream.h"
#include "../../Rendering/Texture2D.h"
#include "../../ImGui/ImGuiRenderer.h"
#include "../../Rendering/Material.h"
#include "../../Rendering/UniformState.h"
#include "Texture2DViewer.h"

struct Texture2DViewer {
    Entity Texture2DViewerTexture, Texture2DViewerMaterial, Texture2DViewerTextureUniformState;
};

void ViewTexture(Entity texture) {
    char editorPath[PathMax];
    snprintf(editorPath, PathMax, "/.editor/Texture2DViewer/%llu", texture);

    auto editor = CreateEntityFromPath(editorPath);
    SetTexture2DViewerTexture(editor, texture);
}

LocalFunction(OnAdded, void, Entity viewer) {
    auto data = GetTexture2DViewer(viewer);

    data->Texture2DViewerMaterial = CreateMaterial(viewer, "Material");
    SetMaterialVertexShader(data->Texture2DViewerMaterial, GetImGuiVertexShader());
    SetMaterialPixelShader(data->Texture2DViewerMaterial, GetImGuiPixelShader());
    SetMaterialDepthTest(data->Texture2DViewerMaterial, RenderState_STATE_DEPTH_TEST_NONE);
    SetMaterialBlendMode(data->Texture2DViewerMaterial, RenderState_STATE_BLEND_NORMAL);

    data->Texture2DViewerTextureUniformState = CreateUniformState(data->Texture2DViewerMaterial, "TextureState");
    SetUniformStateUniform(data->Texture2DViewerTextureUniformState, GetImGuiTextureUniform());
}

LocalFunction(OnChanged, void, Entity viewer) {
    auto data = GetTexture2DViewer(viewer);
    SetUniformStateTexture(data->Texture2DViewerTextureUniformState, data->Texture2DViewerTexture);
}

static void Draw(Entity context) {
    for_entity(viewer, data, Texture2DViewer) {
        auto data = GetTexture2DViewer(viewer);
        auto texture = GetTexture2DViewerTexture(viewer);

        if(!IsEntityValid(texture) || !HasComponent(texture, ComponentOf_Texture2D())) {
            DestroyEntity(viewer);
            continue;
        }

        bool show = true;
        char title[PathMax];
        snprintf(title, PathMax, "%s##%llu", GetEntityPath(texture), viewer);

        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

        if(ImGui::Begin(title, &show)) {
            auto size = ImGui::GetContentRegionAvail();

            ImGui::Image((ImTextureID)(size_t)GetHandleIndex(data->Texture2DViewerMaterial), size);
        }
        ImGui::End();
    }
}

BeginUnit(Texture2DViewer)
    BeginComponent(Texture2DViewer)
        RegisterProperty(Entity, Texture2DViewerTexture)
        DefineExtensionMethod(Texture2D, void, ViewTexture, Entity entity)
EndComponent()
EndUnit()
(Entity, Texture2DViewerTexture)

DefineService(Texture2DViewer)
    RegisterSubscription(ImGuiDraw, Draw, 0)
    RegisterSubscription(Texture2DViewerAdded, OnAdded, 0)
    RegisterSubscription(Texture2DViewerChanged, OnChanged, 0)
EndService()
