//
// Created by Kim on 11-02-2019.
//

#ifndef PLAZA_LAYOUT_H
#define PLAZA_LAYOUT_H

#include <Core/NativeUtils.h>

struct HorizontalLayout {

};

struct VerticalLayout {

};

struct LayoutChildOrdering {
    Entity LayoutOrderingChildProperty;
};

struct Layout {
    v4f LayoutPadding;
    v2f LayoutSpacing;
};

struct LayoutChild {
    v2f LayoutChildWeight;
    v2f LayoutChildMinimumSize;
};

Unit(Layout)
    Component(Layout)
        Property(v4f, LayoutPadding)
        Property(v2f, LayoutSpacing)
        ArrayProperty(LayoutChildOrdering, LayoutChildOrder)

    Component(HorizontalLayout)
    Component(VerticalLayout)

    Component(LayoutChild)
        Property(v2f, LayoutChildWeight)
        Property(v2f, LayoutChildMinimumSize)

#endif //PLAZA_LAYOUT_H
