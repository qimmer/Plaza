//
// Created by Kim Johannsen on 18-03-2018.
//

#ifndef PLAZA_INPUTSTATE_H
#define PLAZA_INPUTSTATE_H


#include <Core/Entity.h>
#include <Core/Service.h>

DeclareComponent(InputState)
DeclareService(InputState)

DeclareComponentPropertyReactive(InputState, u16, InputStateKey)
DeclareComponentPropertyReactive(InputState, u16, InputStatePrimaryModifierKey)
DeclareComponentPropertyReactive(InputState, u16, InputStateSecondaryModifierKey)
DeclareComponentPropertyReactive(InputState, float, InputStateStateScale)

DeclareEvent(CommandPressed, EntityHandler)
DeclareEvent(CommandReleased, EntityHandler)

#endif //PLAZA_INPUTSTATE_H
