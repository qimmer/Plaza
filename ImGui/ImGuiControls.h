#ifndef IMGUICONTROLS_H
#define IMGUICONTROLS_H

#include <ImGui/imgui/imgui.h>

namespace ImGui {
    bool BeginButtonDropDown(const char* label, ImVec2 buttonSize);
    void EndButtonDropDown();
}

#endif