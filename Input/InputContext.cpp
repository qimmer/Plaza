//
// Created by Kim on 15-08-2018.
//

#include "InputContext.h"
#include "Key.h"
#include "AppLoop.h"

struct InputMapping {

};

struct InputState {
    float InputStateValue;
    u16 InputStateKey;
    Entity InputStateContext;
};

struct InputContext {
    v2i InputContextCursorPosition;
    bool InputContextGrabMouse;
    s8 InputContextLastCharacter;
    float InputContextDeadZone;
};

struct HotKey {
    float HotKeyValue;
};

LocalFunction(OnInputStateChanged, void, Entity state) {
    auto hotKey = GetOwnership(state).Owner;
    if(HasComponent(hotKey, ComponentOf_HotKey())) {
        float value = 1.0f;
        for_children(hotKeyState, HotKeyStates, hotKey) {
            if(GetInputStateValue(hotKeyState) < 0.5f) {
                value = 0.0f;
            }
        }

        SetHotKeyValue(hotKey, value);
    }
}

API_EXPORT void SetInputStateValueByKey(Entity context, u16 key, float value) {
    for_entity(inputState, ComponentOf_InputState()) {
        if(!data->InputStateContext || data->InputStateContext == context) {
            if(data->InputStateKey == key) {
                SetInputStateValue(inputState, value);
            }
        }
    }
}

BeginUnit(InputContext)
    BeginComponent(InputContext)
        RegisterProperty(bool, InputContextGrabMouse)
        RegisterProperty(v2i, InputContextCursorPosition)
        RegisterProperty(s8, InputContextLastCharacter)
        RegisterProperty(float, InputContextDeadZone)
    EndComponent()
    BeginComponent(InputState)
        RegisterProperty(float, InputStateValue)
        RegisterPropertyEnum(u16, InputStateKey, Key)
        RegisterReferenceProperty(InputContext, InputStateContext)
    EndComponent()
    BeginComponent(HotKey)
        RegisterArrayProperty(InputState, HotKeyStates)
        RegisterProperty(float, HotKeyValue)
    EndComponent()
    BeginComponent(InputMapping)
        RegisterArrayProperty(InputState, InputMappings)
    EndComponent()

    SetAppLoopOrder(AppLoopOf_InputPoll(), AppLoopOrder_Input);

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InputStateValue()), OnInputStateChanged, 0)
EndUnit()
