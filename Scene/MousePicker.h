//
// Created by Kim Johannsen on 30-03-2018.
//

#ifndef PLAZA_MOUSEPICKER_H
#define PLAZA_MOUSEPICKER_H

#include <Core/Entity.h>

DeclareComponent(MousePicker)
DeclareComponentPropertyReactive(MousePicker, v4f, MousePickerViewport)
DeclareComponentPropertyReactive(MousePicker, Entity, MousePickerContext)

DeclareService(MousePicker)

#endif //PLAZA_MOUSEPICKER_H
