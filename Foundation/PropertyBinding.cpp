//
// Created by Kim Johannsen on 27-02-2018.
//

#include "PropertyBinding.h"

struct PropertyBinding {
    PropertyBinding() {
        memset(this, 0, sizeof(PropertyBinding));
    }

    Entity BindingSource, BindingDestination;
    Property BindingSourceProperty, BindingDestinationProperty;
};

DefineComponent(PropertyBinding)
EndComponent()

DefineComponentProperty(PropertyBinding, Entity, BindingSource)
DefineComponentProperty(PropertyBinding, Entity, BindingDestination)
DefineComponentProperty(PropertyBinding, Property, BindingSourceProperty)
DefineComponentProperty(PropertyBinding, Property, BindingDestinationProperty)
