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

BeginUnit(MenuItem)
    BeginComponent(MenuItem)
    RegisterProperty(StringRef, MenuItemTitle))
    RegisterProperty(u16, MenuItemIcon))
    RegisterProperty(bool, MenuItemSelected))
    RegisterProperty(bool, MenuItemEnabled))
EndComponent()

RegisterProperty(StringRef, MenuItemTitle)
RegisterProperty(u16, MenuItemIcon)
RegisterProperty(bool, MenuItemSelected)
RegisterProperty(bool, MenuItemEnabled)
