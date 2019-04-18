//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_DOCK_H
#define PLAZA_DOCK_H

#include <Core/NativeUtils.h>

struct Dock {
    StringRef DockTitle;
    Entity DockTitleBar;
};

struct DockStyle {
    Entity DockStyleMesh;
    Entity DockStyleFont;
    v4i DockStylePadding;
};

Unit(Dock)
    Component(DockStyle)
        ChildProperty(WidgetMesh, DockStyleMesh)
        ReferenceProperty(Font, DockStyleFont)
        Property(v4i, DockStylePadding)

    Component(Dock)
        Property(StringRef, DockTitle)

    Prefab(Dock)

#endif //PLAZA_DOCK_H
