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

struct VerticalSeparator {
};

struct HorizontalSeparator {
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

struct SeparatorStyle {
    v4i SeparatorStylePadding;
    Entity SeparatorStyleVerticalMesh, SeparatorStyleHorizontalMesh;
};

Unit(Menu)
    Component(Menu)

    Component(MenuItem)
        Property(StringRef, MenuItemTitle)
        ChildProperty(TextWidget, MenuItemLabel)
        ChildProperty(Menu, MenuItemSubMenu)

    Component(MainMenu)

    Component(MainMenuItem)

    Component(MainMenuItemStyle)
        ChildProperty(WidgetMesh, MainMenuItemStyleMesh)

    Component(MenuItemStyle)
        ChildProperty(WidgetMesh, MenuItemStyleMesh)
        Property(v4i, MenuItemStylePadding)

    Component(MainMenuStyle)
        Property(v4i, MainMenuStylePadding)

    Component(SeparatorStyle)
        Property(v4i, SeparatorStylePadding)
        ChildProperty(WidgetMesh, SeparatorStyleVerticalMesh)
        ChildProperty(WidgetMesh, SeparatorStyleHorizontalMesh)

    Prefab(VerticalSeparator)
    Prefab(HorizontalSeparator)
    Prefab(Menu)
    Prefab(MainMenu)
    Prefab(MenuItem)
    Prefab(MainMenuItem)

#endif //PLAZA_MENU_H
