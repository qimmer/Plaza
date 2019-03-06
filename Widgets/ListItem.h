//
// Created by Kim on 25-02-2019.
//

#ifndef PLAZA_LISTITEM_H
#define PLAZA_LISTITEM_H

#include <Core/NativeUtils.h>

struct ListItem {
    Entity ListItemContainer;
    StringRef ListItemTitle, ListItemIcon;
    bool ListItemExpanded;
};

struct ListItemStyle {
    v4i ListItemStylePadding;
    Entity ListItemStyleMesh, ListItemPlusStyleMesh;
};

Unit(ListItem)
    Component(ListItem)
        ChildProperty(Layout, ListItemContainer)
        Property(StringRef, ListItemTitle)
        Property(StringRef, ListItemIcon)
        Property(bool, ListItemExpanded)

    Component(ListItemStyle)
        ChildProperty(WidgetMesh, ListItemStyleMesh)
        Property(v4i, ListItemStylePadding)

#endif //PLAZA_LISTITEM_H
