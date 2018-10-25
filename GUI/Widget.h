//
// Created by Kim Johannsen on 03-04-2018.
//

#ifndef PLAZA_WIDGET_H
#define PLAZA_WIDGET_H

#include <Core/NativeUtils.h>

enum {
    WidgetChildLayout_Horizontal,
    WidgetChildLayout_Vertical
};

enum {
    WidgetSizing_Weighted,
    WidgetSizing_Fixed
};

struct Widget {
    v2i WidgetSize, WidgetMinimumSize;
    v2f WidgetWeight;
    u8 WidgetChildLayout, WidgetSizing;
};

struct WidgetMesh {
    Entity WidgetMeshTexture;
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

    Component(WidgetMesh)
        ReferenceProperty(Texture2D, WidgetMeshTexture)

#endif //PLAZA_WIDGET_H
