//
// Created by Kim Johannsen on 19-03-2018.
//

#include "State.h"

struct State {
    float StateValue;
};

DefineComponent(State)
    DefinePropertyReactive(float, StateValue)
EndComponent()

DefineComponentPropertyReactive(State, float, StateValue)