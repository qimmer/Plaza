//
// Created by Kim on 04-11-2018.
//

#ifndef PLAZA_BUTTON_H
#define PLAZA_BUTTON_H

#include <Core/NativeUtils.h>

struct Button {
    Entity ButtonAction;
};

Unit(Button)
    Component(Button)
        ChildProperty(Invocation, ButtonAction)

#endif //PLAZA_BUTTON_H
