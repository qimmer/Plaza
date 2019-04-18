//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_PANEL_H
#define PLAZA_PANEL_H

#include <Core/NativeUtils.h>

struct PanelStyle {
    v4i PanelStylePadding;
    Entity PanelStyleMesh;
};

struct WellStyle {
    v4i WellStylePadding;
    Entity WellStyleMesh;
};

struct ItemWellStyle {
    v4i ItemWellStylePadding;
    Entity ItemWellStyleMesh;
};

Unit(Panel)
    Component(PanelStyle)
        ChildProperty(WidgetMesh, PanelStyleMesh)
        Property(v4i, PanelStylePadding)

    Component(WellStyle)
        ChildProperty(WidgetMesh, WellStyleMesh)
        Property(v4i, WellStylePadding)

    Component(ItemWellStyle)
        ChildProperty(WidgetMesh, ItemWellStyleMesh)
        Property(v4i, ItemWellStylePadding)

    Prefab(Panel)
    Prefab(Well)
    Prefab(ItemWell)

#endif //PLAZA_PANEL_H
