//
// Created by Kim Johannsen on 04-03-2018.
//

#include <Core/Service.h>
#include <ImGui/ImGuiRenderer.h>
#include "EditorStyle.h"
#include <imgui/imgui.h>
#include <Foundation/Stream.h>
#include <Foundation/VirtualPath.h>


ImFont *UIFont, *MonoFont;

struct ImFont *GetMonospaceFont() {
    return MonoFont;
}

struct ImFont *GetUIFont() {
    return UIFont;
}

static void OnServiceStart(Service service) {
    ImGui::SetCurrentContext((ImGuiContext*)GetDefaultImGuiContext());

    ImGuiIO& io = ImGui::GetIO();

    ImWchar awesomeRange[] = {61440, 62463, 0};
    ImWchar materialRange[] = {57344, 60236, 0};

    ImFontConfig config;
    config.MergeMode = true;

    io.Fonts->Clear();
    UIFont = io.Fonts->AddFontFromFileTTF("./Fonts/DroidSans.ttf", 13.0f);
    io.Fonts->AddFontFromFileTTF("./Fonts/fontawesome-webfont.ttf", 14.0f, &config, awesomeRange);
    io.Fonts->AddFontFromFileTTF("./Fonts/MaterialIcons-Regular.ttf", 14.0f, &config, materialRange);
    //MonoFont = io.Fonts->AddFontFromFileTTF("./Fonts/Courier Prime Code.ttf", 14.0f);
    RebuildImGuiFonts();
}

DefineService(EditorStyle)
        ServiceDependency(ImGuiRenderer)
        Subscribe(EditorStyleStarted, OnServiceStart)
EndService()
