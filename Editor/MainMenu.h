//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_MAINMENU_H
#define PLAZA_MAINMENU_H

#include <Core/Service.h>

DeclareService(MainMenu)

void MainMenuPush(Service service, StringRef text);
void MainMenuPop();
void MainMenuItem(StringRef text, Handler func);
void MainMenuSeparator();

#define PushMenu(TEXT) MainMenuPush(service, TEXT);
#define PopMenu() MainMenuPop();
#define MenuItem(TEXT, FUNCTION)
#define MenuSeparator()

#endif //PLAZA_MAINMENU_H
