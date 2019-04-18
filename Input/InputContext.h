//
// Created by Kim on 15-08-2018.
//

#ifndef PLAZA_INPUTCONTEXT_H
#define PLAZA_INPUTCONTEXT_H

#include <Core/NativeUtils.h>


struct InputState {
    float InputStateValue;
    u16 InputStateKey;
};

struct InputContext {
    ChildArray InputContextStates;
    v2i InputContextCursorPosition;
    float InputContextDeadZone;
    bool InputContextGrabMouse;
    s8 InputContextLastCharacter;
};

Unit(InputContext)
    Component(InputState)
        ReferenceProperty(InputContext, InputStateContext)
        Property(u16, InputStateKey)
        Property(float, InputStateValue)

    Component(InputContext)
        Property(bool, InputContextGrabMouse)
        Property(v2i, InputContextCursorPosition)
        Property(s8, InputContextLastCharacter)
        Property(float, InputContextDeadZone)
        ArrayProperty(InputState, InputContextStates)

#endif //PLAZA_INPUTCONTEXT_H
