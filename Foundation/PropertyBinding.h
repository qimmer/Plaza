//
// Created by Kim Johannsen on 27-02-2018.
//

#ifndef PLAZA_PROPERTYBINDING_H
#define PLAZA_PROPERTYBINDING_H

#include <Core/Entity.h>
#include <Core/Property.h>

DeclareComponent(PropertyBinding)

DeclareComponentProperty(PropertyBinding, Entity, BindingSource)
DeclareComponentProperty(PropertyBinding, Entity, BindingDestination)
DeclareComponentProperty(PropertyBinding, Property, BindingSourceProperty)
DeclareComponentProperty(PropertyBinding, Property, BindingDestinationProperty)

#endif //PLAZA_PROPERTYBINDING_H
