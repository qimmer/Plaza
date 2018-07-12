//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_ImGui_H
#define PLAZA_ImGui_H

#include <Core/NativeUtils.h>

Unit(ImGuiRenderer)
    Component(ImGuiRenderer)

Event(ImGuiDraw);

void* GetDefaultImGuiContext();
void RebuildImGuiFonts();

Entity GetImGuiTextureUniform();
Entity GetImGuiVertexShader();
Entity GetImGuiPixelShader();

#endif //PLAZA_ImGui_H
