//
// Created by Kim Johannsen on 04-03-2018.
//

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

LocalFunction(OnServiceStart, void, Service service) {
    ImGui::SetCurrentContext((ImGuiContext*)GetDefaultImGuiContext());

    ImGuiIO& io = ImGui::GetIO();

    ImWchar awesomeRange[] = {61440, 62463, 0};
    ImWchar materialRange[] = {57344, 60236, 0};

    ImFontConfig config;
    config.MergeMode = true;

    io.Fonts->Clear();
    UIFont = io.Fonts->AddFontFromFileTTF("./Assets/Fonts/DroidSans.ttf", 13.0f);
    io.Fonts->AddFontFromFileTTF("./Assets/Fonts/fontawesome-webfont.ttf", 14.0f, &config, awesomeRange);
    io.Fonts->AddFontFromFileTTF("./Assets/Fonts/MaterialIcons-Regular.ttf", 14.0f, &config, materialRange);
    //MonoFont = io.Fonts->AddFontFromFileTTF("./Fonts/Courier Prime Code.ttf", 14.0f);
    RebuildImGuiFonts();
}

DefineService(EditorStyle)
        ServiceDependency(ImGuiRenderer)
        RegisterSubscription(EditorStyleStarted, OnServiceStart, 0)
EndService()
