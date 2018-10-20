//
// Created by Kim on 15-08-2018.
//

#include "InputContext.h"
#include "Key.h"

struct InputState {
    float InputStateValue;
    u16 InputStateKey;
};

struct InputContext {
    v2i InputContextCursorPosition;
    bool InputContextGrabMouse;
    s8 InputContextLastCharacter;
};

API_EXPORT Entity FindInputState(Entity context, u16 key) {
    for_children(inputState, InputContextStates, context, {
        if(GetInputStateKey(inputState) == key) {
            return inputState;
        }
    });

    return 0;
}

API_EXPORT void SetInputStateValueByKey(Entity context, u16 key, float value) {
    for_children(inputState, InputContextStates, context, {
        if(GetInputStateKey(inputState) == key) {
            SetInputStateValue(inputState, value);
        }
    });
}

BeginUnit(InputContext)
    BeginComponent(InputContext)
        RegisterProperty(bool, InputContextGrabMouse)
        RegisterProperty(v2i, InputContextCursorPosition)
        RegisterArrayProperty(InputState, InputContextStates)
        RegisterProperty(s8, InputContextLastCharacter)
    EndComponent()
    BeginComponent(InputState)
        RegisterProperty(float, InputStateValue)
        RegisterPropertyEnum(u16, InputStateKey, Key)
    EndComponent()
EndUnit()
