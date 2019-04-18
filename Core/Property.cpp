//
// Created by Kim Johannsen on 02/01/2018.
//

#include <Core/NativeUtils.h>
#include <Core/Enum.h>
#include "Function.h"
#include "Debug.h"
#include "Entity.h"
#include "Vector.h"
#include "Enum.h"
#include "Identification.h"
#include "Strings.h"
#include "Algorithms.h"
#include "Property.h"
#include "Instance.h"
#include "System.h"

#include <EASTL/map.h>

#include <malloc.h>
#include <memory.h>
#include <cstring>

#define Verbose_Children "children"
#define Verbose_PropertyChanges "changes"

API_EXPORT Variant GetPropertyValue(Entity property, Entity entity) {
    auto component = GetOwnership(property).Owner;
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto propertyData = GetProperty(property);
    auto data = (char*)const_cast<void*>(GetComponentInstanceData(componentInfoIndex, GetComponentIndexByIndex(componentInfoIndex, entity)));

    Variant var;
    memcpy(&var.data, data + propertyData.PropertyOffset, GetTypeSize(propertyData.PropertyType));
    var.type = propertyData.PropertyType;

    return var;
}

API_EXPORT void SetPropertyValue(Entity property, Entity entity, Variant value) {
    auto component = GetOwnership(property).Owner;
    auto componentData = GetComponent(component);
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto componentIndex = GetComponentIndexByIndex(componentInfoIndex, entity);
    auto propertyData = GetProperty(property);
    auto data = (char*)const_cast<void*>(GetComponentInstanceData(componentInfoIndex, componentIndex));

    auto newData = (char*)alloca(componentData.ComponentSize);
    memcpy(newData, data, componentData.ComponentSize);

    memcpy(newData + propertyData.PropertyOffset, &value.data, GetTypeSize(propertyData.PropertyType));

    SetComponentInstanceData(componentInfoIndex, componentIndex, newData);
}

static void LayoutProperties(Entity component) {
    auto offset = 0;

    auto componentData = GetComponent(component);
    for(auto property : componentData.Properties) {
        auto data = GetProperty(property);

        auto alignment = GetTypeAlignment(data.PropertyType);
        auto size = GetTypeSize(data.PropertyType);

        offset = Align(offset, alignment);

        data.PropertyOffset = offset;

        offset += size;
    }

    offset = Align(offset, alignof(max_align_t)); // Align component size to maximum possible alignment

    if(componentData.ComponentSize != offset) {
        componentData.ComponentSize = offset;
        SetComponent(component, componentData);
    }
}

static void OnPropertyChanged(Entity property, const Property& oldValue, const Property& value) {
    auto component = GetOwnership(property).Owner;
    auto componentData = GetComponent(component);

    // Handle child and array deletion and initialization when kind changes
    if(value.PropertyType != oldValue.PropertyType) {
        auto propertyData = GetProperty(property);

        auto *data = (char*)alloca(componentData.ComponentSize);
        for_entity_data(entity, component, data) {
            if (oldValue.PropertyType == TypeOf_ChildArray) {
                ChildArray& arr = *(ChildArray*) (data + propertyData.PropertyOffset);
                arr.SetSize(0);
            } else if (oldValue.PropertyType == TypeOf_Entity) {
                Entity *child = (Entity *) (data + propertyData.PropertyOffset);

                auto childOwnership = GetOwnership(*child);
                if(childOwnership.Owner == entity && childOwnership.OwnerProperty == property) {
                    DestroyEntity(*child);
                }

                *child = 0;
            }
            SetComponentData(entity, component, data);
        }

        LayoutProperties(component);
    }

    if(value.PropertyOffset != oldValue.PropertyOffset) {
        auto *data = (char*)alloca(componentData.ComponentSize);
        for_entity_data(entity, component, data) {
            memmove(data + value.PropertyOffset, data + oldValue.PropertyOffset, GetTypeSize(oldValue.PropertyType));
        }
    }
}

static void OnComponentChanged(Entity component, const Component& oldValue, const Component& newValue) {
    if(newValue.ComponentExplicitSize) {
        LayoutProperties(component);
    }
}

static void OnArrayChildChanged(Entity entity, const ArrayChild& oldValue, const ArrayChild& newValue) {
    SetIdentification(entity, {StringFormatV("%s.%s", GetIdentification(GetOwnership(entity).Owner), newValue.Name)});
}

BeginUnit(Property)
    RegisterSystem(OnPropertyChanged, ComponentOf_Property())
    RegisterSystem(OnComponentChanged, ComponentOf_Component())
    RegisterSystem(OnArrayChildChanged, ComponentOf_ArrayChild())

    BeginComponent(Property)
        RegisterProperty(u32, PropertyOffset)
        RegisterProperty(u32, PropertySize)
        RegisterProperty(Type, PropertyType)
        RegisterProperty(bool, PropertyReadOnly)
        RegisterReferenceProperty(Enum, PropertyEnum)
        RegisterProperty(Entity, PropertyPrefab)
    EndComponent()
    BeginComponent(Ownership)
        RegisterProperty(Entity, Owner)
        RegisterReferenceProperty(Property, OwnerProperty)
    EndComponent()
    BeginComponent(ArrayChild)
        RegisterProperty(StringRef, Name)
    EndComponent()

    RegisterFunction(SetPropertyValue)
    RegisterFunction(GetPropertyValue)
EndUnit()

static void DumpEntity(Entity entity, u32 indentLevel) {
    for(u32 i = 0; i < indentLevel; ++i) printf("%s", "  ");
    printf("%s\n", GetIdentification(entity).Uuid);

    for_entity(child, ComponentOf_Identification()) {
        if(GetOwnership(child).Owner == entity) {
            DumpEntity(child, indentLevel+1);
        }
    }
}

API_EXPORT void DumpTree(Entity entity) {
    DumpEntity(entity, 0);
}

API_EXPORT bool IsOwnedBy(Entity entity, Entity owner) {
    auto entityOwner = GetOwnership(entity).Owner;
    return IsEntityValid(entityOwner) && (entityOwner == owner || IsOwnedBy(entityOwner, owner));
}
