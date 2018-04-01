//
// Created by Kim Johannsen on 01-04-2018.
//

#include <Core/String.h>
#include "MenuItem.h"

struct MenuItem {
    String MenuItemTitle;
    u16 MenuItemIcon;
    Entity MenuItemSelectedState, MenuItemEnabledState;
};

DefineComponent(MenuItem)
    DefineProperty(StringRef, MenuItemTitle)
    DefineProperty(u16, MenuItemIcon)
    DefineProperty(Entity, MenuItemSelectedState)
    DefineProperty(Entity, MenuItemEnabledState)
EndComponent()

DefineComponentPropertyReactive(MenuItem, StringRef, MenuItemTitle)
DefineComponentPropertyReactive(MenuItem, u16, MenuItemIcon)
DefineComponentPropertyReactive(MenuItem, Entity, MenuItemSelectedState)
DefineComponentPropertyReactive(MenuItem, Entity, MenuItemEnabledState)
