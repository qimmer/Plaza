//
// Created by Kim Johannsen on 18-03-2018.
//

#include <Logic/State.h>
#include <Rendering/Context.h>
#include "InputState.h"

struct InputState {
    InputState() : InputStateStateScale(1.0f), InputStateKey(0), InputStatePrimaryModifierKey(0), InputStateSecondaryModifierKey(0) {
    }

    u16 InputStateKey, InputStatePrimaryModifierKey, InputStateSecondaryModifierKey;
    float InputStateStateScale;
};

static void OnStateValueChanged(Entity state, float oldState, float newState) {
    if(HasInputState(state)) {
        if(oldState < 0.5f && newState > 0.5f) {
            FireEvent(CommandPressed, state);
        }

        if(oldState > 0.5f && newState < 0.5f) {
            FireEvent(CommandReleased, state);
        }
    }
}

static void OnChanged(Entity entity) {
    auto data = GetInputState(entity);
    Assert(data->InputStateKey < KEY_MAX);
    Assert(data->InputStatePrimaryModifierKey < KEY_MAX);
    Assert(data->InputStateSecondaryModifierKey < KEY_MAX);
}

DefineEvent(CommandPressed, EntityHandler)
DefineEvent(CommandReleased, EntityHandler)

DefineComponent(InputState)
    Dependency(State)
    DefineProperty(u16, InputStateKey)
    DefineProperty(u16, InputStatePrimaryModifierKey)
    DefineProperty(u16, InputStateSecondaryModifierKey)
    DefineProperty(float, InputStateStateScale)
EndComponent()

DefineComponentPropertyReactive(InputState, u16, InputStateKey)
DefineComponentPropertyReactive(InputState, float, InputStateStateScale)
DefineComponentPropertyReactive(InputState, u16, InputStatePrimaryModifierKey)
DefineComponentPropertyReactive(InputState, u16, InputStateSecondaryModifierKey)

DefineService(InputState)
    Subscribe(StateValueChanged, OnStateValueChanged)
    Subscribe(InputStateChanged, OnChanged)
EndService()

static bool ServiceStart() {
    return true;
}

static bool ServiceStop() {
    return true;
}