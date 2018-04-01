//
// Created by Kim Johannsen on 31-03-2018.
//

#include "InvertState.h"
#include "State.h"

struct InvertState {
    Entity InvertStateSourceState;
};

DefineComponent(InvertState)
    DefinePropertyReactive(Entity, InvertStateSourceState)
EndComponent()

DefineComponentPropertyReactive(InvertState, Entity, InvertStateSourceState)

static void OnChanged(Entity entity) {
    auto sourceState = GetInvertStateSourceState(entity);
    if(IsEntityValid(sourceState)) {
        SetStateValue(entity, 1.0f - GetStateValue(sourceState));
    }

}

static void OnStateChanged(Entity state, float oldState, float newState) {
    for_entity(invertState, InvertState) {
        auto data = GetInvertState(invertState);
        if(data->InvertStateSourceState == state) {
            SetStateValue(invertState, 1.0f - GetStateValue(state));
        }
    }
}

DefineService(InvertState)
    Subscribe(InvertStateChanged, OnChanged)
    Subscribe(StateValueChanged, OnStateChanged)
EndService()