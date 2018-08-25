//
// Created by Kim Johannsen on 31-03-2018.
//

#include <Rendering/RenderContext.h>
#include <Input/InputState.h>
#include <ImGui/imgui/imgui.h>
#include <ImGui/ImGuiRenderer.h>
#include <Input/Key.h>
#include "EditorContext.h"
#include "EditorView.h"
#include "MainMenu.h"

Entity EditorMainContext;

struct EditorContext {
    EditorContext() : EditorContextVisible(true) {

    }
    bool EditorContextVisible;
};

BeginUnit(EditorContext)
    BeginComponent(EditorContext)
EndComponent()
EndUnit()
(bool, EditorContextVisible)

LocalFunction(OnEditorContextAdded, void, Entity editor) {
    auto data = GetEditorContext(editor);

    for(auto type = GetNextType(0); type; type = GetNextType(type)) {
        if(IsComponent(type)) {
            for(auto i = 0; i < GetDependencies(type); ++i) {
                if(GetDependency(type, i) == TypeOf_EditorView()) {
                    auto view = CreateNode(editor, GetTypeName(type));
                    AddComponent(view, type);
                    break;
                }
            }
        }
    }

    auto mainMenu = CreateMainMenu(editor, "MainMenu");
}

LocalFunction(OnImGuiDraw, void, Entity editor) {
    if(HasComponent(editor, ComponentOf_EditorContext())) {
        if(GetEditorContextVisible(editor)) {
            for_children(view, editor) {
                if(HasComponent(view, ComponentOf_EditorView())
                   && GetEditorViewVisible(view)
                   && GetEditorViewDrawFunction(view)) {
                    auto open = true;
                    if(ImGui::Begin(GetName(view), &open)) {
                        GetEditorViewDrawFunction(view)(view);
                    }
                    if(!open) {
                        SetEditorViewVisible(view, false);
                    }
                    ImGui::End();
                }
            }
        }
    }
}

static void CreateEditorMainContext(Entity context) {
    SetContextTitle(context, "Editor");
    AddEditorContext(context);
}

DefineService(EditorContext)
    RegisterSubscription(EditorContextAdded, OnEditorContextAdded, 0)
    RegisterSubscription(ImGuiDraw, OnImGuiDraw, 0)

    ServiceEntity(EditorMainContext, CreateEditorMainContext)
EndService()