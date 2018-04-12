//
// Created by Kim Johannsen on 03-04-2018.
//

#ifndef PLAZA_WIDGET_H
#define PLAZA_WIDGET_H

#include <Core/Entity.h>

DeclareEvent(WidgetMouseDown, EntityHandler)
DeclareEvent(WidgetMouseUp, EntityHandler)

DeclareComponent(Widget)
DeclareComponentPropertyReactive(Widget, v2i, WidgetSize)
DeclareComponentPropertyReactive(Widget, Entity, WidgetTexture)

#endif //PLAZA_WIDGET_H
