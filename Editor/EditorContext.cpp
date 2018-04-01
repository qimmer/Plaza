//
// Created by Kim Johannsen on 31-03-2018.
//

#include <Rendering/Context.h>
#include <Logic/ToggleState.h>
#include <Logic/State.h>
#include <Input/InputState.h>
#include <ImGui/imgui/imgui.h>
#include <ImGui/ImGuiRenderer.h>
#include <Input/Key.h>
#include "EditorContext.h"
#include "EditorView.h"
#include "MainMenu.h"

struct EditorContext {
    Entity EditorContextVisibilityState, EditorContextVisibilityCommand;
};

DefineComponent(EditorContext)
EndComponent()

DefineComponentPropertyReactive(EditorContext, Entity, EditorContextVisibilityState)
DefineComponentPropertyReactive(EditorContext, Entity, EditorContextVisibilityCommand)

static void OnEditorContextAdded(Entity editor) {
    auto data = GetEditorContext(editor);

    data->EditorContextVisibilityState = CreateToggleState(editor, "VisibilityState");
    data->EditorContextVisibilityCommand = CreateInputState(editor, "VisibilityCommand");
    SetToggleStateSourceState(data->EditorContextVisibilityState, data->EditorContextVisibilityCommand);
    SetStateValue(data->EditorContextVisibilityState, 0.0f);
    SetInputStateKey(data->EditorContextVisibilityCommand, KEY_F8);

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
    SetMainMenuVisibilityState(mainMenu, data->EditorContextVisibilityState);

}

static void OnImGuiDraw(Entity editor) {
    if(GetStateValue(GetEditorContextVisibilityState(editor)) > 0.5f) {
        for_children(view, editor) {
            if(HasEditorView(view)
               && GetStateValue(GetEditorViewVisibilityState(view)) > 0.5f
               && GetEditorViewDrawFunction(view)) {
                auto open = true;
                if(ImGui::Begin(GetName(view), &open)) {
                    GetEditorViewDrawFunction(view)(view);
                }
                if(!open) {
                    SetStateValue(GetEditorViewVisibilityState(view), 0.0f);
                }
                ImGui::End();
            }
        }
    }
}

static void OnStarted(Service service) {
    AddExtension(TypeOf_Context(), TypeOf_EditorContext());
}

static void OnStopped(Service service) {
    RemoveExtension(TypeOf_Context(), TypeOf_EditorContext());
}

DefineService(EditorContext)
    Subscribe(EditorContextAdded, OnEditorContextAdded)
    Subscribe(EditorContextStarted, OnStarted)
    Subscribe(EditorContextStopped, OnStopped)
    Subscribe(ImGuiDraw, OnImGuiDraw)
EndService()