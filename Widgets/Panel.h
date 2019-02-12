//
// Created by Kim on 17-01-2019.
//

#ifndef PLAZA_PANEL_H
#define PLAZA_PANEL_H

#include <Core/NativeUtils.h>

struct Panel {
};

struct PanelStyle {
    v4i PanelStylePadding;
    Entity PanelStyleMesh;
};

struct Well {
};

struct WellStyle {
    v4i WellStylePadding;
    Entity WellStyleMesh;
};

Unit(Panel)
    Component(Panel)

    Component(PanelStyle)
        ChildProperty(WidgetMesh, PanelStyleMesh)
        Property(v4i, PanelStylePadding)

    Component(Well)

    Component(WellStyle)
        ChildProperty(WidgetMesh, WellStyleMesh)
        Property(v4i, WellStylePadding)

#endif //PLAZA_PANEL_H
