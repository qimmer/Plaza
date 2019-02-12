//
// Created by Kim on 11-02-2019.
//

#ifndef PLAZA_MENU_H
#define PLAZA_MENU_H

#include <Core/NativeUtils.h>

struct Menu {
};

struct MainMenu {

};

struct MenuItem {
    StringRef MenuItemTitle;
};

struct Separator {

};

Unit(Menu)
    Component(Menu)

    Component(MainMenu)

    Component(MenuItem)
        Property(StringRef, MenuItemTitle)

    Component(Separator)

#endif //PLAZA_MENU_H
