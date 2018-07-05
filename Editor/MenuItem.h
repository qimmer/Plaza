//
// Created by Kim Johannsen on 01-04-2018.
//

#ifndef PLAZA_MENUITEM_H
#define PLAZA_MENUITEM_H


#include <Core/Entity.h>

DeclareComponent(MenuItem)
DeclareComponentPropertyReactive(MenuItem, StringRef, MenuItemTitle)
DeclareComponentPropertyReactive(MenuItem, u16, MenuItemIcon)
DeclareComponentPropertyReactive(MenuItem, bool, MenuItemSelected)
DeclareComponentPropertyReactive(MenuItem, bool, MenuItemEnabled)

#endif //PLAZA_MENUITEM_H
