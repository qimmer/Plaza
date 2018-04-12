//
// Created by Kim Johannsen on 05-04-2018.
//

#ifndef PLAZA_PROPERTYBINDING_H
#define PLAZA_PROPERTYBINDING_H

#include <Core/Entity.h>

DeclareComponent(PropertyBinding)
DeclareComponentPropertyReactive(PropertyBinding, Entity, PropertyBindingSourceEntity)
DeclareComponentPropertyReactive(PropertyBinding, Entity, PropertyBindingDestinationEntity)
DeclareComponentPropertyReactive(PropertyBinding, Property, PropertyBindingSourceProperty)
DeclareComponentPropertyReactive(PropertyBinding, Property, PropertyBindingDestinationProperty)

DeclareService(PropertyBinding)

#endif //PLAZA_PROPERTYBINDING_H
