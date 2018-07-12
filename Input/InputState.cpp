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

LocalFunction(OnStateValueChanged, void, Entity state, float oldState, float newState) {
    if(oldState < 0.5f && newState > 0.5f) {
        FireEvent(EventOf_CommandPressed(), state);
    }

    if(oldState > 0.5f && newState < 0.5f) {
        FireEvent(EventOf_CommandReleased(), state);
    }
}

LocalFunction(OnChanged, void, Entity entity) {
    auto data = GetInputStateData(entity);
    Assert(data->InputStateKey < KEY_MAX);
    Assert(data->InputStatePrimaryModifierKey < KEY_MAX);
    Assert(data->InputStateSecondaryModifierKey < KEY_MAX);
}

RegisterEvent(CommandPressed)
RegisterEvent(CommandReleased)

BeginUnit(InputState)
    BeginComponent(InputState)
    DefinePropertyReactiveEnum(u16, InputStateKey, Key)
    DefinePropertyReactiveEnum(u16, InputStatePrimaryModifierKey, Key)
    DefinePropertyReactiveEnum(u16, InputStateSecondaryModifierKey, Key)
    RegisterProperty(float, InputStateValue)
EndComponent()

RegisterProperty(u16, InputStateKey)
RegisterProperty(float, InputStateValue)
RegisterProperty(u16, InputStatePrimaryModifierKey)
RegisterProperty(u16, InputStateSecondaryModifierKey)

DefineService(InputState)
    RegisterSubscription(InputStateValueChanged, OnStateValueChanged, 0)
    RegisterSubscription(InputStateChanged, OnChanged, 0)
EndService()
