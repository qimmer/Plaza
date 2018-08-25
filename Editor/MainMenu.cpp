//
// Created by Kim Johannsen on 17/01/2018.
//

#include "MainMenu.h"
#include "MenuItem.h"
#include <ImGuiRenderer.h>
#include <Core/Entity.h>
#include <imgui/imgui.h>
#include <Input/InputState.h>

struct MainMenu {
    bool MainMenuVisible;
};

BeginUnit(MainMenu)
    BeginComponent(MainMenu)
EndComponent()
EndUnit()
(bool, MainMenuVisible)

static void DrawMenu(Entity menu) {
    char title[256];
    snprintf(title, 256, "%s", GetMenuItemTitle(menu));

    auto enabled = GetMenuItemEnabled(menu);
    auto selected = GetMenuItemSelected(menu);

    if(GetFirstChild(menu)) {
        if(ImGui::BeginMenu(title, enabled)) {
            for(auto subMenu = GetFirstChild(menu); subMenu; subMenu = GetSibling(subMenu)) {
                if(!HasComponent(subMenu, ComponentOf_MenuItem())) continue;

                DrawMenu(subMenu);
            }

            ImGui::EndMenu();
        }
    } else {
        if(ImGui::MenuItem(title, NULL, selected, enabled)) {
            SetInputStateValue(menu, 1.0f);
            SetInputStateValue(menu, 0.0f);
        }
    }
}
LocalFunction(OnImGuiDraw, void, Entity context) {
    for(auto mainMenu = GetFirstChild(context); mainMenu; mainMenu = GetSibling(mainMenu)) {
        if(HasComponent(mainMenu, ComponentOf_MainMenu())) {
            auto visible = GetMainMenuVisible(mainMenu);
            if(!visible) continue;

            if(ImGui::BeginMainMenuBar()) {
                for(auto menu = GetFirstChild(mainMenu); menu; menu = GetSibling(menu)) {
                    if(!HasComponent(menu, ComponentOf_MenuItem())) continue;

                    DrawMenu(menu);
                }
                ImGui::EndMainMenuBar();
            }
        }
    }
}

DefineService(MainMenu)
    RegisterSubscription(ImGuiDraw, OnImGuiDraw, 0)
EndService()
