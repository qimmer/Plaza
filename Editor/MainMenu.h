//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_MAINMENU_H
#define PLAZA_MAINMENU_H

#include <Core/Service.h>
#include <Core/Entity.h>

DeclareService(MainMenu)

DeclareComponent(MainMenu)
DeclareComponentPropertyReactive(MainMenu, bool, MainMenuVisible)

#endif //PLAZA_MAINMENU_H
