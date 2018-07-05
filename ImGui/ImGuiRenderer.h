//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_ImGui_H
#define PLAZA_ImGui_H

#include <Core/Service.h>
#include <Core/Entity.h>

DeclareService(ImGuiRenderer)
DeclareComponent(ImGuiRenderer)

DeclareEvent(ImGuiDraw, Entity entity);

void* GetDefaultImGuiContext();
void RebuildImGuiFonts();

Entity GetImGuiTextureUniform();
Entity GetImGuiVertexShader();
Entity GetImGuiPixelShader();

#endif //PLAZA_ImGui_H
