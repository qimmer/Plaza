//
// Created by Kim Johannsen on 31-03-2018.
//

#include "PropertyState.h"
#include "State.h"
#include <Core/Property.h>

typedef float(*StateConversionHandler)(Entity entity, Property property);

struct PropertyState {
    Entity PropertyStateTarget;
    Property PropertyStateProperty;
    StateConversionHandler ConversionFunc;
};

DefineComponent(PropertyState)
    DefinePropertyReactive(Entity, PropertyStateTarget)
    DefinePropertyReactive(Property, PropertyStateProperty)
EndComponent()

DefineComponentPropertyReactive(PropertyState, Entity, PropertyStateTarget)
DefineComponentPropertyReactive(PropertyState, Property, PropertyStateProperty)

static float ConvertFromBool(Entity entity, Property property) {
    auto getter = (EntityBoolHandler)GetPropertyGetter(property);
    return getter(entity) ? 1.0f : 0.0f;
}

static float ConvertFromFloat(Entity entity, Property property) {
    auto getter = (EntityFloatHandler)GetPropertyGetter(property);
    return getter(entity);
}

static void OnComponentChanged(Entity entity, Type componentType) {
    for_entity_data(propertyState, data, PropertyState) {
        if(data->PropertyStateProperty
           && entity == data->PropertyStateTarget
           && GetPropertyOwner(data->PropertyStateProperty) == componentType
           && data->ConversionFunc) {
            SetStateValue(entity, data->ConversionFunc(entity, data->PropertyStateProperty));
            break;
        }
    }
}

static void OnPropertyStatePropertyChanged(Entity entity, Property oldValue, Property newValue) {
    auto data = GetPropertyState(entity);
    if(IsPropertyValid(oldValue)) {
        data->ConversionFunc = NULL;
    }
    if(IsPropertyValid(newValue)) {
        if(GetPropertyType(newValue) == TypeOf_bool()) data->ConversionFunc = ConvertFromBool;
        else if(GetPropertyType(newValue) == TypeOf_float()) data->ConversionFunc = ConvertFromFloat;
    }
}

DefineService(PropertyState)
    Subscribe(PropertyStatePropertyChanged, OnPropertyStatePropertyChanged)
EndService()