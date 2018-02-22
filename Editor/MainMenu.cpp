//
// Created by Kim Johannsen on 17/01/2018.
//

#include "MainMenu.h"
#include <ImGuiRenderer.h>
#include <Core/Entity.h>


    DefineService(MainMenu)
    EndService()

    Entity MainMenuRoot;

    void MainMenuPush(Service service, StringRef text) {

    }

    void MainMenuPop() {

    }

    void MainMenuItem(StringRef text, Handler func) {

    }

    void MainMenuSeparator() {

    }

    static bool ServiceStart() {

        return true;
    }

    static bool ServiceStop() {

        return true;
    }
