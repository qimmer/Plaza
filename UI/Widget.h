//
// Created by Kim Johannsen on 03-04-2018.
//

#ifndef PLAZA_WIDGET_H
#define PLAZA_WIDGET_H

#include <Core/Entity.h>

DeclareEnum(WidgetChildLayout)
#define WidgetChildLayout_Horizontal 0
#define WidgetChildLayout_Vertical 1

DeclareEnum(WidgetSizing)
#define WidgetSizing_Weighted 0
#define WidgetSizing_Fixed 1

DeclareEvent(WidgetMouseDown, EntityHandler)
DeclareEvent(WidgetMouseUp, EntityHandler)

DeclareComponent(Widget)
DeclareComponentPropertyReactive(Widget, v2i, WidgetMinimumSize)
DeclareComponentPropertyReactive(Widget, v2f, WidgetWeight)
DeclareComponentPropertyReactive(Widget, u8, WidgetChildLayout)
DeclareComponentPropertyReactive(Widget, u8, WidgetSizing)
DeclareComponentPropertyReactive(Widget, v2i, WidgetSize)

DeclareService(Widget)

#endif //PLAZA_WIDGET_H
