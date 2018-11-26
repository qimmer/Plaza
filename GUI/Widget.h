//
// Created by Kim Johannsen on 03-04-2018.
//

#ifndef PLAZA_WIDGET_H
#define PLAZA_WIDGET_H

#include <Core/NativeUtils.h>

enum {
    WidgetChildLayout_Manual,
    WidgetChildLayout_Horizontal,
    WidgetChildLayout_Vertical
};

struct Widget {
    v2i WidgetSize, WidgetMinimumSize, WidgetPadding, WidgetSpacing;
    v2f WidgetWeight;
    u8 WidgetChildLayout;
};

struct InteractableWidget {
    bool WidgetHovered, WidgetFocused, WidgetClicked, WidgetDisabled;
    Entity WidgetHoveredSubMesh, WidgetFocusedSubMesh, WidgetClickedSubMesh, WidgetDisabledSubMesh, WidgetEnabledSubMesh;
    v2i WidgetInteractionPoint;
};

struct WidgetMesh {
    Entity WidgetMeshTexture;
    u16 WidgetMeshFixedBorderWidth;
};

Unit(Widget)
    Enum(WidgetChildLayout)
    Enum(WidgetSizing)

    Component(Widget)
        Property(v2i, WidgetMinimumSize)
        Property(v2f, WidgetWeight)
        Property(u8, WidgetChildLayout)
        Property(u8, WidgetSizing)
        Property(v2i, WidgetSize)
        Property(v4i, WidgetPadding)
        Property(v2i, WidgetSpacing)

    Component(InteractableWidget)
        Property(v2i, WidgetInteractionPoint)
        Property(bool, WidgetHovered)
        Property(bool, WidgetFocused)
        Property(bool, WidgetClicked)
        Property(bool, WidgetDisabled)
        ReferenceProperty(SubMesh, WidgetHoveredSubMesh)
        ReferenceProperty(SubMesh, WidgetFocusedSubMesh)
        ReferenceProperty(SubMesh, WidgetClickedSubMesh)
        ReferenceProperty(SubMesh, WidgetDisabledSubMesh)
        ReferenceProperty(SubMesh, WidgetEnabledSubMesh)

    Component(WidgetMesh)
        ReferenceProperty(SubTexture2D, WidgetMeshTexture)
        Property(u16, WidgetMeshFixedBorderWidth)


#endif //PLAZA_WIDGET_H
