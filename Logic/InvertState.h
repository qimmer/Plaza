//
// Created by Kim Johannsen on 31-03-2018.
//

#ifndef PLAZA_INVERTSTATE_H
#define PLAZA_INVERTSTATE_H

#include <Core/Entity.h>

DeclareComponent(InvertState)
DeclareComponentPropertyReactive(InvertState, Entity, InvertStateSourceState)

DeclareService(InvertState)

#endif //PLAZA_INVERTSTATE_H
