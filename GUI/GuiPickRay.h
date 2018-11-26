//
// Created by Kim on 01-11-2018.
//

#ifndef PLAZA_GUIPICKRAY_H
#define PLAZA_GUIPICKRAY_H

#include <Core/NativeUtils.h>

struct GuiPickRay {
    Entity GuiPickRayClickState, GuiPickRayPickedWidget;
};

Unit(GuiPickRay)
    Component(GuiPickRay)
        ChildProperty(InputState, GuiPickRayClickState)
        ReferenceProperty(Widget, GuiPickRayPickedWidget)

#endif //PLAZA_GUIPICKRAY_H
