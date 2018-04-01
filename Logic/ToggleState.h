//
// Created by Kim Johannsen on 19-03-2018.
//

#ifndef PLAZA_TOGGLESTATE_H
#define PLAZA_TOGGLESTATE_H

#include <Core/Entity.h>

DeclareComponent(ToggleState)
DeclareComponentPropertyReactive(ToggleState, Entity, ToggleStateSourceState)

DeclareService(ToggleState)

#endif //PLAZA_TOGGLESTATE_H
