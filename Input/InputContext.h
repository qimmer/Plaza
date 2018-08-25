//
// Created by Kim on 15-08-2018.
//

#ifndef PLAZA_INPUTCONTEXT_H
#define PLAZA_INPUTCONTEXT_H

#include <Core/NativeUtils.h>

Unit(InputContext)
    Component(InputState)
        Property(u16, InputStateKey)
        Property(float, InputStateValue)

    Component(InputContext)
        Property(bool, InputContextGrabMouse)
        Property(v2i, InputContextCursorPosition)
        Property(s8, InputContextLastCharacter)
        ArrayProperty(KeyState, InputContextStates)

    Function(FindInputState, Entity, Entity context, u16 key)
    Function(SetInputStateValueByKey, void, Entity context, u16 key, float value)

#endif //PLAZA_INPUTCONTEXT_H
