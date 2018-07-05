//
// Created by Kim Johannsen on 01-04-2018.
//

#include <Core/String.h>
#include "MenuItem.h"

struct MenuItem {
    String MenuItemTitle;
    u16 MenuItemIcon;
    bool MenuItemSelected, MenuItemEnabled;
};

DefineComponent(MenuItem)
    DefineProperty(StringRef, MenuItemTitle)
    DefineProperty(u16, MenuItemIcon)
    DefineProperty(bool, MenuItemSelected)
    DefineProperty(bool, MenuItemEnabled)
EndComponent()

DefineComponentPropertyReactive(MenuItem, StringRef, MenuItemTitle)
DefineComponentPropertyReactive(MenuItem, u16, MenuItemIcon)
DefineComponentPropertyReactive(MenuItem, bool, MenuItemSelected)
DefineComponentPropertyReactive(MenuItem, bool, MenuItemEnabled)
