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

struct MainMenuItem {
    StringRef MainMenuItemTitle;
    Entity MainMenuItemLabel, MainMenuItemSubMenu;
};

struct MenuItem {
    StringRef MenuItemTitle;
    Entity MenuItemLabel, MenuItemSubMenu;
};

struct Separator {

};

struct MainMenuItemStyle {
    Entity MainMenuItemStyleMesh;
};

struct MenuItemStyle {
    Entity MenuItemStyleMesh;
    v4i MenuItemStylePadding;
};

struct MainMenuStyle {
    v4i MainMenuStylePadding;
};

Unit(Menu)
    Component(Menu)
        ArrayProperty(MenuItem, MenuItems)

    Component(MenuItem)
        Property(StringRef, MenuItemTitle)
        ChildProperty(TextWidget, MenuItemLabel)
        ChildProperty(Menu, MenuItemSubMenu)

    Component(MainMenu)

    Component(MainMenuItem)

    Component(Separator)

    Component(MainMenuItemStyle)
        ChildProperty(WidgetMesh, MainMenuItemStyleMesh)

    Component(MenuItemStyle)
        ChildProperty(WidgetMesh, MenuItemStyleMesh)
        Property(v4i, MenuItemStylePadding)

    Component(MainMenuStyle)
        Property(v4i, MainMenuStylePadding)

#endif //PLAZA_MENU_H
