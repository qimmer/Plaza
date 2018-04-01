//
// Created by Kim Johannsen on 19-03-2018.
//

#include "ToggleState.h"
#include "State.h"

struct ToggleState {
    Entity ToggleStateSourceState;
};

DefineComponent(ToggleState)
    DefinePropertyReactive(Entity, ToggleStateSourceState)
EndComponent()

DefineComponentPropertyReactive(ToggleState, Entity, ToggleStateSourceState)

static void OnStateChanged(Entity state, float oldState, float newState) {
    // Check if the state is going from ON to OFF
    if(oldState > 0.5f && newState <= 0.5f) {
        for_entity(toggleState, ToggleState) {
            auto data = GetToggleState(toggleState);
            if(data->ToggleStateSourceState == state) {
                SetStateValue(toggleState, 1.0f - (GetStateValue(toggleState) > 0.5f ? 1.0f : 0.0f));
            }
        }
    }
}

DefineService(ToggleState)
    Subscribe(StateValueChanged, OnStateChanged)
EndService()