//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_ImGui_H
#define PLAZA_ImGui_H

#include <Core/Service.h>
#include <Core/Entity.h>

DeclareService(ImGuiRenderer)
DeclareComponent(ImGuiRenderer)

DeclareEvent(ImGuiDraw, EntityHandler);

void* GetDefaultImGuiContext();
void RebuildImGuiFonts();

#endif //PLAZA_ImGui_H
