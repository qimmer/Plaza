//
// Created by Kim Johannsen on 18-03-2018.
//

#include <Rendering/Context.h>
#include "InputState.h"
#include "Key.h"

struct InputState {
    InputState() : InputStateValue(0.0f), InputStateKey(0), InputStatePrimaryModifierKey(0), InputStateSecondaryModifierKey(0) {
    }

    u16 InputStateKey, InputStatePrimaryModifierKey, InputStateSecondaryModifierKey;
    float InputStateValue;
};

static void OnStateValueChanged(Entity state, float oldState, float newState) {
    if(oldState < 0.5f && newState > 0.5f) {
        FireNativeEvent(CommandPressed, state);
    }

    if(oldState > 0.5f && newState < 0.5f) {
        FireNativeEvent(CommandReleased, state);
    }
}

static void OnChanged(Entity entity) {
    auto data = GetInputState(entity);
    Assert(data->InputStateKey < KEY_MAX);
    Assert(data->InputStatePrimaryModifierKey < KEY_MAX);
    Assert(data->InputStateSecondaryModifierKey < KEY_MAX);
}

DefineEvent(CommandPressed)
DefineEvent(CommandReleased)

DefineComponent(InputState)
    DefinePropertyReactiveEnum(u16, InputStateKey, Key)
    DefinePropertyReactiveEnum(u16, InputStatePrimaryModifierKey, Key)
    DefinePropertyReactiveEnum(u16, InputStateSecondaryModifierKey, Key)
    DefinePropertyReactive(float, InputStateValue)
EndComponent()

DefineComponentPropertyReactive(InputState, u16, InputStateKey)
DefineComponentPropertyReactive(InputState, float, InputStateValue)
DefineComponentPropertyReactive(InputState, u16, InputStatePrimaryModifierKey)
DefineComponentPropertyReactive(InputState, u16, InputStateSecondaryModifierKey)

DefineService(InputState)
    Subscribe(InputStateValueChanged, OnStateValueChanged)
    Subscribe(InputStateChanged, OnChanged)
EndService()
