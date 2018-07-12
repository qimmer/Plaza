//
// Created by Kim Johannsen on 01-04-2018.
//

#ifndef PLAZA_MENUITEM_H
#define PLAZA_MENUITEM_H


#include <Core/Entity.h>

Unit(MenuItem)
    Component(MenuItem)
        Property(StringRef, MenuItemTitle)
        Property(u16, MenuItemIcon)
        Property(bool, MenuItemSelected)
        Property(bool, MenuItemEnabled)

#endif //PLAZA_MENUITEM_H
