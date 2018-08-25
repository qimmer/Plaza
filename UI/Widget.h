//
// Created by Kim Johannsen on 03-04-2018.
//

#ifndef PLAZA_WIDGET_H
#define PLAZA_WIDGET_H

#include <Core/NativeUtils.h>

Enum(WidgetChildLayout)
#define WidgetChildLayout_Horizontal 0
#define WidgetChildLayout_Vertical 1

Enum(WidgetSizing)
#define WidgetSizing_Weighted 0
#define WidgetSizing_Fixed 1

Event(WidgetPress)
Event(WidgetRelease)
Event(WidgetEnter)
Event(WidgetLeave)

Unit(Widget)
    Component(Widget)
        Property(v2i, WidgetMinimumSize)
        Property(v2f, WidgetWeight)
        Property(u8, WidgetChildLayout)
        Property(u8, WidgetSizing)
        Property(v2i, WidgetSize)
        Property(Entity, WidgetTexture)
DeclareComponentChild(Widget, WidgetTextureState)
DeclareComponentChild(Widget, WidgetSizeState)
DeclareComponentChild(Widget, WidgetBorderSizeState)


#endif //PLAZA_WIDGET_H
