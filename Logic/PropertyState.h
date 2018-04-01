//
// Created by Kim Johannsen on 31-03-2018.
//

#ifndef PLAZA_PROPERTYSTATE_H
#define PLAZA_PROPERTYSTATE_H

#include <Core/Entity.h>

DeclareComponent(PropertyState)
DeclareComponentPropertyReactive(PropertyState, Entity, PropertyStateTarget)
DeclareComponentPropertyReactive(PropertyState, Property, PropertyStateProperty)

DeclareService(PropertyState)

#endif //PLAZA_PROPERTYSTATE_H
