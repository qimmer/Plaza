//
// Created by Kim on 25-02-2019.
//

#ifndef PLAZA_LISTITEM_H
#define PLAZA_LISTITEM_H

#include <Core/NativeUtils.h>

struct ListItem {
    Entity ListItemLabel, ListItemChildContainer;
    StringRef ListItemTitle;
};

struct ListItemStyle {
    v4i ListItemStylePadding;
    Entity ListItemStyleMesh;
};

Unit(ListItem)
    Component(ListItem)
        ChildProperty(Widget, ListItemLabel)
        ChildProperty(Layout, ListItemChildContainer)
        Property(StringRef, ListItemTitle)

    Component(ListItemStyle)
        ChildProperty(WidgetMesh, ListItemStyleMesh)
        Property(v4i, ListItemStylePadding)

#endif //PLAZA_LISTITEM_H
