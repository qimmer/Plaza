//
// Created by Kim on 01-11-2018.
//

#ifndef PLAZA_GUICONTROLLER_H
#define PLAZA_GUICONTROLLER_H

#include <Core/NativeUtils.h>

struct GuiController {
    Entity GuiControllerWidgetPicker, GuiControllerScrollablePicker;
    bool GuiControllerLeftClicked, GuiControllerMiddleClicked, GuiControllerRightClicked, GuiControllerScrollUp, GuiControllerScrollDown;
    s8 GuiControllerLastCharacter;
};

Unit(GuiController)
    Component(GuiController)
        Property(bool, GuiControllerLeftClicked)
        Property(bool, GuiControllerMiddleClicked)
        Property(bool, GuiControllerRightClicked)
        Property(bool, GuiControllerScrollUp)
        Property(bool, GuiControllerScrollDown)
        Property(s8, GuiControllerLastCharacter)
        ChildProperty(TraceRay, GuiControllerWidgetPicker)
        ChildProperty(TraceRay, GuiControllerScrollablePicker)

#endif //PLAZA_GUIPICKRAY_H
