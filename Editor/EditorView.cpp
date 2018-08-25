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

BeginUnit(EditorView)
    BeginComponent(EditorView)
EndComponent()
EndUnit()
(bool, EditorViewVisible)
RegisterProperty(EntityHandler, EditorViewDrawFunction)
