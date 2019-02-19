//
// Created by Kim on 11-02-2019.
//

#ifndef PLAZA_LAYOUT_H
#define PLAZA_LAYOUT_H

#include <Core/NativeUtils.h>

enum {
    LayoutMode_Vertical,
    LayoutMode_Horizontal,
    LayoutMode_Stacked
};

struct LayoutChildOrdering {
    Entity LayoutChildOrderingProperty;
};

struct Layout {
    v4i LayoutPadding;
    v2i LayoutSpacing;
    u8 LayoutMode;
};

struct LayoutChild {
    v2f LayoutChildWeight;
};

Unit(Layout)
    Enum(LayoutMode)
    Component(Layout)
        Property(v4i, LayoutPadding)
        Property(v2i, LayoutSpacing)
        Property(u8, LayoutMode)
        ArrayProperty(LayoutChildOrdering, LayoutChildOrder)

    Component(LayoutChildOrdering)
        ReferenceProperty(Property, LayoutChildOrderingProperty)

    Component(LayoutChild)
        Property(v2f, LayoutChildWeight)

#endif //PLAZA_LAYOUT_H
