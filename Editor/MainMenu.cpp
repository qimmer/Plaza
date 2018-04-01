//
// Created by Kim Johannsen on 17/01/2018.
//

#include "MainMenu.h"
#include "MenuItem.h"
#include <ImGuiRenderer.h>
#include <Core/Entity.h>
#include <imgui/imgui.h>
#include <Logic/State.h>

struct MainMenu {
    Entity MainMenuVisibilityState;
};

DefineComponent(MainMenu)
EndComponent()

DefineComponentPropertyReactive(MainMenu, Entity, MainMenuVisibilityState)

static void DrawMenu(Entity menu) {
    char title[256];
    snprintf(title, 256, "%s", GetMenuItemTitle(menu));

    auto enabledState = GetMenuItemEnabledState(menu);
    auto selectedState = GetMenuItemSelectedState(menu);

    auto enabled = IsEntityValid(enabledState) ? (GetStateValue(enabledState) > 0.5f) : true;
    auto selected = IsEntityValid(selectedState) ? (GetStateValue(selectedState) > 0.5f) : false;

    if(GetFirstChild(menu)) {
        if(ImGui::BeginMenu(title, enabled)) {
            for(auto subMenu = GetFirstChild(menu); subMenu; subMenu = GetSibling(subMenu)) {
                if(!HasMenuItem(subMenu)) continue;

                DrawMenu(subMenu);
            }

            ImGui::EndMenu();
        }
    } else {
        if(ImGui::MenuItem(title, NULL, selected, enabled)) {
            SetStateValue(menu, 1.0f);
            SetStateValue(menu, 0.0f);
        }
    }
}
static void OnImGuiDraw(Entity context) {
    for(auto mainMenu = GetFirstChild(context); mainMenu; mainMenu = GetSibling(mainMenu)) {
        if(HasMainMenu(mainMenu)) {
            auto visibilityState = GetMainMenuVisibilityState(mainMenu);
            if(IsEntityValid(visibilityState) && GetStateValue(visibilityState) < 0.5f) continue;

            if(ImGui::BeginMainMenuBar()) {
                for(auto menu = GetFirstChild(mainMenu); menu; menu = GetSibling(menu)) {
                    if(!HasMenuItem(menu)) continue;

                    DrawMenu(menu);
                }
                ImGui::EndMainMenuBar();
            }
        }
    }
}

DefineService(MainMenu)
    Subscribe(ImGuiDraw, OnImGuiDraw)
EndService()
