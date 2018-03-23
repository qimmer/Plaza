//
// Created by Kim Johannsen on 19-03-2018.
//

#ifndef PLAZA_STATEBINDING_H
#define PLAZA_STATEBINDING_H

#include <Core/Entity.h>

DeclareComponent(StateBinding)
DeclareComponentPropertyReactive(StateBinding, Entity, StateBindingTarget)
DeclareComponentPropertyReactive(StateBinding, Entity, StateBindingProperty)

#endif //PLAZA_STATEBINDING_H
