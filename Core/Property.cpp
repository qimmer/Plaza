//
// Created by Kim Johannsen on 02/01/2018.
//

#include <Core/Property.h>
#include <Core/Component.h>
#include <Core/Hierarchy.h>
#include <malloc.h>
#include <memory.h>
#include <cstring>
#include "Function.h"
#include "Debug.h"
#include "Entity.h"

struct Property {
    Entity PropertyComponent, PropertyEnum, PropertyChangedEvent;
    u32 PropertyOffset, PropertySize;
    Type PropertyType;
};

struct Binding {
    Entity BindingSourceEntity, BindingSourceProperty, BindingTargetEntity, BindingTargetProperty;
};

API_EXPORT void SetPropertyValue(Entity property, Entity context, const void *newValueData) {
    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);
    auto propertyData = (Property*)GetComponentData(ComponentOf_Property(), propertyIndex);

    if(!propertyData) {
        Log(context, LogSeverity_Error, "Property has not been registered.");
        return;
    }

    Assert(property, IsEntityValid(propertyData->PropertyComponent));
    AddComponent(context, propertyData->PropertyComponent);

    auto offset = propertyData->PropertyOffset;

    auto componentIndex = GetComponentIndex(propertyData->PropertyComponent, context);
    auto componentData = GetComponentData(propertyData->PropertyComponent, componentIndex);
    Assert(property, componentData);
    auto valueData = componentData + offset;

    const Type argumentTypes[] = {TypeOf_Entity, propertyData->PropertyType, propertyData->PropertyType};
    const void * argumentData[] = {&context, valueData, newValueData};

    // If property data is a char array, change valueData to point to a char pointer (StringRef) instead of the actual char data.
    if(propertyData->PropertySize != GetTypeSize(propertyData->PropertyType) && propertyData->PropertyType != TypeOf_unknown) {
        argumentData[1] = (char*)&valueData;
        FireEventFast(propertyData->PropertyChangedEvent, 3, argumentTypes, argumentData);
        strcpy(valueData, *(const char**)newValueData);
    } else {
        FireEventFast(propertyData->PropertyChangedEvent, 3, argumentTypes, argumentData);
        memcpy(valueData, newValueData, propertyData->PropertySize);
    }
}

API_EXPORT bool GetPropertyValue(Entity property, Entity context, void *dataOut) {
    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);
    auto propertyData = (Property*)GetComponentData(ComponentOf_Property(), propertyIndex);

    if(!propertyData) return false;

    auto offset = propertyData->PropertyOffset;

    auto componentIndex = GetComponentIndex(propertyData->PropertyComponent, context);
    auto componentData = GetComponentData(propertyData->PropertyComponent, componentIndex);

    if(componentIndex == InvalidIndex || !componentData) {
        if(propertyData->PropertySize != GetTypeSize(propertyData->PropertyType)) {
            *(const char**)dataOut = "";
        } else {
            memset(dataOut, 0, GetTypeSize(propertyData->PropertyType));
        }

        return false;
    }

    auto valueData = componentData + offset;

    if(propertyData->PropertySize != GetTypeSize(propertyData->PropertyType)) {
        *(const char**)dataOut = valueData;
    } else {
        memcpy(dataOut, valueData, propertyData->PropertySize);
    }

    return true;
}

BeginUnit(Property)
    BeginComponent(Property)
        RegisterProperty(u32, PropertyOffset)
        RegisterProperty(u32, PropertySize)
        RegisterProperty(Type, PropertyType)
        RegisterProperty(Entity, PropertyEnum)
        RegisterProperty(Entity, PropertyComponent)
        RegisterProperty(Entity, PropertyChangedEvent)
    EndComponent()
    BeginComponent(Binding)
        RegisterProperty(Entity, BindingSourceEntity)
        RegisterProperty(Entity, BindingSourceProperty)
        RegisterProperty(Entity, BindingTargetEntity)
        RegisterProperty(Entity, BindingTargetProperty)
    EndComponent()
EndUnit()

void __Property(Entity property, u32 offset, u32 size, Type type, Entity component) {
    AddComponent(property, ComponentOf_Property());
    auto data = GetProperty(property);
    data->PropertyOffset = offset;
    data->PropertySize = size;
    data->PropertyType = type;
    data->PropertyComponent = component;
}

void __InitializeProperty() {
    auto component = ComponentOf_Property();
    __Property(PropertyOf_PropertyOffset(), offsetof(Property, PropertyOffset), sizeof(Property::PropertyOffset), TypeOf_u32,  component);
    __Property(PropertyOf_PropertySize(),   offsetof(Property, PropertySize),   sizeof(Property::PropertySize),   TypeOf_u32,  component);
    __Property(PropertyOf_PropertyType(),   offsetof(Property, PropertyType),   sizeof(Property::PropertyType),   TypeOf_Type, component);
    __Property(PropertyOf_PropertyComponent(), offsetof(Property, PropertyComponent), sizeof(Property::PropertyComponent), TypeOf_Entity,  component);
    __Property(PropertyOf_PropertyChangedEvent(), offsetof(Property, PropertyChangedEvent), sizeof(Property::PropertyChangedEvent), TypeOf_Entity,  component);
}