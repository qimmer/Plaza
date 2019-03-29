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

typedef struct {
    u32 Count;
    u32 DynCapacity;
    Entity * DynBuf;
    Entity StaBuf [ChildArrayStaticCap];
} ChildArrayData;

API_EXPORT u32 GetArrayPropertyCount(Entity property, Entity entity) {
    auto component = GetOwnership(property).Owner;
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto propertyData = GetProperty(property);
    auto data = (const char*)GetComponentInstanceData(componentInfoIndex, GetComponentIndexByIndex(componentInfoIndex, entity));

    const ChildArrayData& arrayData = *(ChildArrayData*)(data + propertyData.PropertyOffset);
    return arrayData.Count;
}

API_EXPORT u32 AddArrayPropertyElement(Entity property, Entity entity, Entity element) {
    auto component = GetOwnership(property).Owner;
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto propertyData = GetProperty(property);
    auto data = (char*)const_cast<void*>(GetComponentInstanceData(componentInfoIndex, GetComponentIndexByIndex(componentInfoIndex, entity)));

    ChildArrayData& arrayData = *(ChildArrayData*)(data + propertyData.PropertyOffset);
    auto index = arrayData.Count;
    SetVectorAmount(arrayData, arrayData.Count + 1);
    GetVector(arrayData)[index] = element;

    auto ownership = GetOwnership(element);
    if(!ownership.Owner || !ownership.OwnerProperty) {
        ownership.Owner = entity;
        ownership.OwnerProperty = property;
    }

    return index;
}

API_EXPORT void RemoveArrayPropertyElement(Entity property, Entity entity, u32 index) {
    auto component = GetOwnership(property).Owner;
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto propertyData = GetProperty(property);
    auto data = (char*)const_cast<void*>(GetComponentInstanceData(componentInfoIndex, GetComponentIndexByIndex(componentInfoIndex, entity)));

    ChildArrayData& arrayData = *(ChildArrayData*)(data + propertyData.PropertyOffset);

    if(index < arrayData.Count) {
        auto element = GetVector(arrayData)[index];

        auto ownership = GetOwnership(element);
        if(ownership.Owner == entity && ownership.OwnerProperty == property) {
            ownership.Owner = 0;
            ownership.OwnerProperty = 0;
        }

        // Replace removal slot with last element and pop last slot
        GetVector(arrayData)[index] = GetVector(arrayData)[arrayData.Count - 1];

        SetVectorAmount(arrayData, arrayData.Count - 1);
    }
}

API_EXPORT u32 GetArrayPropertyIndex(Entity property, Entity entity, Entity element) {
    auto component = GetOwnership(property).Owner;
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto propertyData = GetProperty(property);
    auto data = (char*)const_cast<void*>(GetComponentInstanceData(componentInfoIndex, GetComponentIndexByIndex(componentInfoIndex, entity)));

    ChildArrayData& arrayData = *(ChildArrayData*)(data + propertyData.PropertyOffset);

    auto v = GetVector(arrayData);
    for(auto i = 0; i < arrayData.Count; ++i) {
        if(v[i] == element) {
            return i;
        }
    }

    return InvalidIndex;
}

API_EXPORT void SetArrayPropertyCount(Entity property, Entity entity, u32 count) {
    auto existingCount = GetArrayPropertyCount(property, entity);

    if(count > existingCount) {
        for(auto i = existingCount; i < count; ++i) {
            AddArrayPropertyElement(property, entity, 0);
        }
    } else if (count < existingCount) {
        while(existingCount > count) {
            RemoveArrayPropertyElement(property, entity, existingCount - 1);
            existingCount--;
        }
    }
}

API_EXPORT const Entity *GetArrayPropertyElements(Entity property, Entity entity) {
    auto component = GetOwnership(property).Owner;
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto propertyData = GetProperty(property);
    auto data = (char*)const_cast<void*>(GetComponentInstanceData(componentInfoIndex, GetComponentIndexByIndex(componentInfoIndex, entity)));

    ChildArrayData& arrayData = *(ChildArrayData*)(data + propertyData.PropertyOffset);

    return GetVector(arrayData);
}

API_EXPORT Entity GetArrayPropertyElement(Entity property, Entity entity, u32 index) {
    auto component = GetOwnership(property).Owner;
    auto componentInfoIndex = GetComponentIndexByIndex(0, component);
    auto propertyData = GetProperty(property);
    auto data = (char*)const_cast<void*>(GetComponentInstanceData(componentInfoIndex, GetComponentIndexByIndex(componentInfoIndex, entity)));

    ChildArrayData& arrayData = *(ChildArrayData*)(data + propertyData.PropertyOffset);

    return GetVector(arrayData)[index];
}

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

    for_children(property, PropertyOf_Properties(), component) {
        auto data = GetProperty(property);

        auto alignment = GetTypeAlignment(data.PropertyType);
        auto size = GetTypeSize(data.PropertyType);

        offset = Align(offset, alignment);

        data.PropertyOffset = offset;

        offset += size;
    }

    offset = Align(offset, alignof(max_align_t)); // Align component size to maximum possible alignment

    auto componentData = GetComponent(component);
    if(componentData.ComponentSize != offset) {
        componentData.ComponentSize = offset;
        SetComponent(component, componentData);
    }
}

static void OnPropertyChanged(Entity property, const Property& value, const Property& oldValue) {
    auto component = GetOwnership(property).Owner;
    auto componentData = GetComponent(component);

    // Handle child and array deletion and initialization when kind changes
    if(value.PropertyType != oldValue.PropertyType) {
        auto propertyData = GetProperty(property);

        auto *data = (char*)alloca(componentData.ComponentSize);
        for_entity_data(entity, component, data) {
            if (oldValue.PropertyType == TypeOf_ChildArray) {
                SetArrayPropertyCount(property, entity, 0);
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

static void OnComponentChanged(Entity component, const Component& value, const Component& oldValue) {
    if(value.ComponentExplicitSize) {
        LayoutProperties(component);
    }
}

static void OnArrayChildChanged(Entity entity, const ArrayChild& value, const ArrayChild& oldValue) {
    SetIdentification(entity, {StringFormatV("%s.%s", GetIdentification(GetOwnership(entity).Owner), value.Name)});
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
        RegisterReferenceProperty(Component, PropertyChildComponent)
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
