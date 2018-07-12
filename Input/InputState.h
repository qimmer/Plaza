//
// Created by Kim Johannsen on 18-03-2018.
//

#ifndef PLAZA_INPUTSTATE_H
#define PLAZA_INPUTSTATE_H


#include <Core/Entity.h>

Unit(InputState)
    Component(InputState)

        Property(u16, InputStateKey)
        Property(u16, InputStatePrimaryModifierKey)
        Property(u16, InputStateSecondaryModifierKey)
        Property(float, InputStateScale)
        Property(float, InputStateValue)

Event(CommandPressed)
Event(CommandReleased)

#endif //PLAZA_INPUTSTATE_H
