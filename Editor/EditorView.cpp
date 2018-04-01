//
// Created by Kim Johannsen on 31-03-2018.
//

#include <Logic/ToggleState.h>
#include <Input/InputState.h>
#include <Logic/State.h>
#include <ImGui/ImGuiRenderer.h>
#include "EditorView.h"
#include <imgui/imgui.h>

struct EditorView {
    Entity EditorViewVisibilityState, EditorViewVisibilityCommand;
    EntityHandler EditorViewDrawFunction;
};

DefineComponent(EditorView)
EndComponent()

DefineComponentPropertyReactive(EditorView, Entity, EditorViewVisibilityState)
DefineComponentPropertyReactive(EditorView, Entity, EditorViewVisibilityCommand)
DefineComponentProperty(EditorView, EntityHandler, EditorViewDrawFunction)

static void OnEditorViewAdded(Entity entity) {
    auto data = GetEditorView(entity);

    data->EditorViewVisibilityState = CreateToggleState(entity, "VisibilityState");
    data->EditorViewVisibilityCommand = CreateInputState(entity, "VisibilityCommand");
    SetToggleStateSourceState(data->EditorViewVisibilityState, data->EditorViewVisibilityCommand);
    SetStateValue(data->EditorViewVisibilityState, 1.0f);
}

DefineService(EditorView)
    Subscribe(EditorViewAdded, OnEditorViewAdded)
EndService()
