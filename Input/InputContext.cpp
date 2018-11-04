//
// Created by Kim on 15-08-2018.
//

#include "InputContext.h"
#include "Key.h"

struct InputState {
    float InputStateValue;
    u16 InputStateKey;
    Entity InputStateContext;
};

struct InputContext {
    v2i InputContextCursorPosition;
    bool InputContextGrabMouse;
    s8 InputContextLastCharacter;
};

API_EXPORT void SetInputStateValueByKey(Entity context, u16 key, float value) {
    for_entity(inputState, data, InputState, {
        if(!data->InputStateContext || data->InputStateContext == context) {
            if(data->InputStateKey == key) {
                SetInputStateValue(inputState, value);
            }
        }
    });
}

BeginUnit(InputContext)
    BeginComponent(InputContext)
        RegisterProperty(bool, InputContextGrabMouse)
        RegisterProperty(v2i, InputContextCursorPosition)
        RegisterProperty(s8, InputContextLastCharacter)
    EndComponent()
    BeginComponent(InputState)
        RegisterProperty(float, InputStateValue)
        RegisterPropertyEnum(u16, InputStateKey, Key)
        RegisterReferenceProperty(InputContext, InputStateContext)
    EndComponent()
EndUnit()
