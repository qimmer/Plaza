//
// Created by Kim on 31-10-2018.
//

#ifndef PLAZA_GUICAMERA_H
#define PLAZA_GUICAMERA_H

#include <Core/NativeUtils.h>

struct GuiCamera {
    Entity GuiCameraPickedWidget;
};

Unit(GuiCamera)
    Component(GuiCamera)
        ReferenceProperty(Widget, GuiCameraHoveredWidget)

#endif //PLAZA_GUICAMERA_H
