//
// Created by Kim Johannsen on 31-03-2018.
//

#include <Rendering/Context.h>
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

DefineComponent(EditorContext)
EndComponent()

DefineComponentPropertyReactive(EditorContext, bool, EditorContextVisible)

static void OnEditorContextAdded(Entity editor) {
    auto data = GetEditorContext(editor);

    for(auto type = GetNextType(0); type; type = GetNextType(type)) {
        if(IsComponent(type)) {
            for(auto i = 0; i < GetDependencies(type); ++i) {
                if(GetDependency(type, i) == TypeOf_EditorView()) {
                    auto view = CreateHierarchy(editor, GetTypeName(type));
                    AddComponent(view, type);
                    break;
                }
            }
        }
    }

    auto mainMenu = CreateMainMenu(editor, "MainMenu");
}

static void OnImGuiDraw(Entity editor) {
    if(HasEditorContext(editor)) {
        if(GetEditorContextVisible(editor)) {
            for_children(view, editor) {
                if(HasEditorView(view)
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
    Subscribe(EditorContextAdded, OnEditorContextAdded)
    Subscribe(ImGuiDraw, OnImGuiDraw)

    ServiceEntity(EditorMainContext, CreateEditorMainContext)
EndService()