//
// Created by Kim Johannsen on 31-03-2018.
//

#include <Input/InputState.h>
#include <ImGui/ImGuiRenderer.h>
#include "EditorView.h"
#include <imgui/imgui.h>

struct EditorView {
    EditorView() : EditorViewVisible(true) {}

    bool EditorViewVisible;
    EntityHandler EditorViewDrawFunction;
};

DefineComponent(EditorView)
EndComponent()

DefineComponentPropertyReactive(EditorView, bool, EditorViewVisible)
DefineComponentProperty(EditorView, EntityHandler, EditorViewDrawFunction)
