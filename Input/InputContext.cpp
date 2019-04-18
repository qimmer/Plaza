//
// Created by Kim on 15-08-2018.
//

#include "InputContext.h"
#include "Key.h"
#include "AppLoop.h"

BeginUnit(InputContext)
    BeginComponent(InputContext)
        RegisterProperty(bool, InputContextGrabMouse)
        RegisterProperty(v2i, InputContextCursorPosition)
        RegisterProperty(s8, InputContextLastCharacter)
        RegisterProperty(float, InputContextDeadZone)
        RegisterArrayProperty(InputState, InputContextStates)
    EndComponent()
    BeginComponent(InputState)
        RegisterProperty(float, InputStateValue)
        RegisterPropertyEnum(u16, InputStateKey, Key)
    EndComponent()
EndUnit()
