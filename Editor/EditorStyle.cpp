//
// Created by Kim Johannsen on 04-03-2018.
//

#include <Core/Service.h>
#include <ImGui/ImGuiRenderer.h>
#include "EditorStyle.h"
#include <imgui/imgui.h>
#include <Foundation/Stream.h>

DefineService(EditorStyle)
    ServiceDependency(ImGuiRenderer)
EndService()

static bool ServiceStart() {
    ImGui::SetCurrentContext((ImGuiContext*)GetDefaultImGuiContext());

    ImGuiIO& io = ImGui::GetIO();

    ImWchar awesomeRange[] = {61440, 62463, 0};
    ImWchar materialRange[] = {57344, 60236, 0};

    ImFontConfig config;
    config.MergeMode = true;

    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("./Fonts/DroidSans.ttf", 13.0f);
    io.Fonts->AddFontFromFileTTF("./Fonts/fontawesome-webfont.ttf", 14.0f, &config, awesomeRange);
    io.Fonts->AddFontFromFileTTF("./Fonts/MaterialIcons-Regular.ttf", 14.0f, &config, materialRange);
    RebuildImGuiFonts();
}

static bool ServiceStop() {

}
