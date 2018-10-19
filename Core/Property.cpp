//
// Created by Kim Johannsen on 02/01/2018.
//

#include <Core/Property.h>
#include <Core/Enum.h>
#include <Core/Component.h>
#include "Function.h"
#include "Debug.h"
#include "Entity.h"
#include "Vector.h"
#include "Enum.h"
#include "Identification.h"
#include "Strings.h"
#include "Algorithms.h"
#include "Property.h"


#include <malloc.h>
#include <memory.h>
#include <cstring>

extern "C" {
    #include <ex_intern/strings.h>
    #include <ex_intern/optimize.h>
}

#define Verbose_Children "children"
#define Verbose_PropertyChanges "changes"

static Entity nullEntity = 0;

struct EntityVectorStruct {
	u32 Count;
	u32 DynCapacity;
	Entity *DynBuf;
	Entity StaBuf[1];
};

struct Ownership {
    Ownership() : Owner(0), OwnerProperty(0) {}

    Entity Owner, OwnerProperty;
};

struct Property {
    Entity PropertyEnum, PropertyChangedEvent, PropertyChildComponent;
    u32 PropertyOffset, PropertySize, PropertyFlags;
    Type PropertyType;
    u8 PropertyKind;
    bool PropertyReadOnly;
};

struct EntityChildren {
    EntityChildren() : StartIndex(0), Count(0) {}

    u32 StartIndex;
    u32 Count;
    eastl::fixed_vector<Entity, 32> children;
};

struct PropertyChildArray {
    eastl::vector<Entity> ChildrenPool;
    eastl::vector<EntityChildren> PerEntityChildrenInfo;
};

static eastl::vector<PropertyChildArray> PerPropertyChildrenInfo;
static eastl::vector<GenericPropertyChangedListener> PropertyChangedListeners;

PropertyChildArray *GetPropertyChildren(Entity property) {
    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);

    if(propertyIndex == InvalidIndex) {
        return NULL;
    }

    if(PerPropertyChildrenInfo.size() <= propertyIndex) {
        PerPropertyChildrenInfo.resize(propertyIndex + 1);
    }

    return &PerPropertyChildrenInfo[propertyIndex];
}

API_EXPORT u32 AddChild(Entity property, Entity entity, Entity child, bool takeOwnership) {
    if(!IsEntityValid(child)) {
        Error(entity, "Cannot add invalid %s child", GetDebugName(property));
        return InvalidIndex;
    }

    auto propertyChildren = GetPropertyChildren(property);
    if(!propertyChildren) {
        return InvalidIndex;
    }

    Verbose(Verbose_Children, "Added %s '%s' to '%s'", GetDebugName(property), GetDebugName(child), GetDebugName(entity));

    auto owner = GetOwner(child);
    auto ownerProperty = GetOwnerProperty(child);

    // If child has current owner, remove child from owner, as we are taking over ownership of this child
    if(takeOwnership && owner != entity && IsEntityValid(owner)) {
        auto index = GetChildIndex(ownerProperty, owner, child);
        RemoveChild(ownerProperty, owner, index);
    }

    auto entityIndex = GetEntityIndex(entity);
    auto& perEntityChildrenInfo = propertyChildren->PerEntityChildrenInfo;
    if(perEntityChildrenInfo.size() <= entityIndex) {
        perEntityChildrenInfo.resize(entityIndex + 1);

        if(perEntityChildrenInfo.size() > 1) {
            perEntityChildrenInfo[entityIndex].StartIndex = perEntityChildrenInfo[entityIndex - 1].StartIndex + perEntityChildrenInfo[entityIndex - 1].Count;
        }
    }

    auto& info = propertyChildren->PerEntityChildrenInfo[entityIndex];

    // Tell all subsequent child arrays to index additional one entity
    for(auto i = entityIndex + 1; i < propertyChildren->PerEntityChildrenInfo.size(); ++i) {
        propertyChildren->PerEntityChildrenInfo[i].StartIndex++;
    }

    // Insert child into end of child array, pushing all subsequent entries one index
    //propertyChildren->ChildrenPool.insert(propertyChildren->ChildrenPool.begin() + info.StartIndex + info.Count, child);

    propertyChildren->PerEntityChildrenInfo[entityIndex].children.push_back(child);

    info.Count++;

    if(takeOwnership) {
        SetOwner(child, entity, property);
    }

    return info.Count - 1;
}

API_EXPORT bool RemoveChild(Entity property, Entity entity, u32 index) {
    auto propertyChildren = GetPropertyChildren(property);
    if(!propertyChildren) return false;

    auto entityIndex = GetEntityIndex(entity);
    if(propertyChildren->PerEntityChildrenInfo.size() <= entityIndex) return false;

    auto& info = propertyChildren->PerEntityChildrenInfo[entityIndex];
    if(index >= info.Count) return false;

    auto child = propertyChildren->PerEntityChildrenInfo[entityIndex].children[index];//propertyChildren->ChildrenPool[info.StartIndex + index];

    Verbose(Verbose_Children, "Removed %s '%s' to '%s'", GetDebugName(property), GetDebugName(child), GetDebugName(entity));

    if(GetOwner(child) == entity) {
        SetOwner(child, 0, 0);
    }

    // Tell all subsequent child arrays to index one entity less
    for(auto i = entityIndex + 1; i < propertyChildren->PerEntityChildrenInfo.size(); ++i) {
        propertyChildren->PerEntityChildrenInfo[i].StartIndex--;
    }

    // Remove child from array, popping all subsequent entries one index
    //propertyChildren->ChildrenPool.erase(propertyChildren->ChildrenPool.begin() + info.StartIndex + index);

    info.Count--;

    propertyChildren->PerEntityChildrenInfo[entityIndex].children.erase(propertyChildren->PerEntityChildrenInfo[entityIndex].children.begin()  + index);

    return true;
}

API_EXPORT u32 GetChildIndex(Entity property, Entity entity, Entity child) {
    u32 count = 0;
    auto children = GetChildren(property, entity, &count);
    if(!children) return InvalidIndex;

    for(auto i = 0; i < count; ++i) {
        if(children[i] == child) return i;
    }

    return InvalidIndex;
}

API_EXPORT Entity* GetChildren(Entity property, Entity entity, u32 *count) {
    auto propertyChildren = GetPropertyChildren(property);
    if(!propertyChildren) return NULL;

    auto entityIndex = GetEntityIndex(entity);
    if(propertyChildren->PerEntityChildrenInfo.size() <= entityIndex) return NULL;

    auto& info = propertyChildren->PerEntityChildrenInfo[entityIndex];

    if(!info.Count) return NULL;

    if(count) {
        *count = propertyChildren->PerEntityChildrenInfo[entityIndex].children.size();
    }

    return propertyChildren->PerEntityChildrenInfo[entityIndex].children.data(); //&propertyChildren->ChildrenPool[info.StartIndex];
}

API_EXPORT void EmitChangedEvent(Entity entity, Entity property, Property *propertyData, const void *oldValueData, const void *newValueData) {
    if(propertyData) {
        const Type argumentTypes[] = {TypeOf_Entity, propertyData->PropertyType, propertyData->PropertyType};
        const void * argumentData[] = {&entity, oldValueData, newValueData};

        FireEventFast(propertyData->PropertyChangedEvent, 3, argumentTypes, argumentData);
    }

    for(auto& listener : PropertyChangedListeners) {
        listener(property, entity, propertyData->PropertyType, oldValueData, newValueData);
    }
}

API_EXPORT void SetPropertyValue(Entity property, Entity context, const void *newValueData) {
    auto component = GetOwner(property);

    static auto componentOfProperty = ComponentOf_Property();
    static StringRef nullStr = Intern("");
    auto propertyIndex = GetComponentIndex(componentOfProperty, property);
    auto propertyData = (Property*) GetComponentBytes(componentOfProperty, propertyIndex);

    if(!propertyData) {
        Log(context, LogSeverity_Error, "Property has not been registered.");
        return;
    }

    AddComponent(context, component);

    auto offset = propertyData->PropertyOffset;
    auto size = GetTypeSize(propertyData->PropertyType);

    auto componentIndex = GetComponentIndex(component, context);
    auto componentData = GetComponentBytes(component, componentIndex);
    Assert(property, componentData);

    auto valueData = componentData + offset;

    StringRef internedString = 0;
    if(propertyData->PropertyType == TypeOf_StringRef) {
        if(!memcmp(valueData, "\0\0\0\0\0\0\0\0", sizeof(StringRef))) {
            *(StringRef*)valueData = nullStr;
        }

        ReleaseStringRef(*(StringRef*)valueData);

        internedString = AddStringRef(*(StringRef *) newValueData);
        newValueData = &internedString;
    }

    if(memcmp(valueData, newValueData, size) != 0) {
        auto oldValueData = alloca(size);
        memcpy(oldValueData, valueData, size);
        memcpy(valueData, newValueData, size);

        EmitChangedEvent(context, property, propertyData, oldValueData, newValueData);
    }
}

API_EXPORT bool GetPropertyValue(Entity property, Entity context, void *dataOut) {
    auto component = GetOwner(property);

    if(!IsEntityValid(property)) {
        Log(context, LogSeverity_Error, "Invalid property when trying to get property");
        return false;
    }

    if(!IsEntityValid(component)) {
        Log(context, LogSeverity_Error, "Invalid component when trying to get property %s", GetDebugName(property));
        return false;
    }

    if(!IsEntityValid(context)) {
        Log(0, LogSeverity_Error, "Invalid entity when trying to get property %s", GetDebugName(property));
        return false;
    }

    static StringRef nullStr = Intern("");
    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);
    auto propertyData = (Property*) GetComponentBytes(ComponentOf_Property(), propertyIndex);

    if(!propertyData) {
        Log(context, LogSeverity_Error, "Property has not been registered.");
        return false;
    }

    auto offset = propertyData->PropertyOffset;
    auto size = GetTypeSize(propertyData->PropertyType);

    auto componentIndex = GetComponentIndex(component, context);
    auto componentData = GetComponentBytes(component, componentIndex);

    if(componentIndex == InvalidIndex || !componentData) {
        if(propertyData->PropertyKind == PropertyKind_Array) {
            *(VectorStruct**)dataOut = 0;
        } else {
            memset(dataOut, 0, size);
        }

        Verbose(Verbose_PropertyChanges, "'%s' is not present on entity '%s'. Returning default value.", GetDebugName(property), GetDebugName(component));

        return false;
    }

    auto valueData = componentData + offset;
    if(propertyData->PropertyKind == PropertyKind_Array) {
        *(VectorStruct**)dataOut = (VectorStruct*)valueData;
    } else {
        if(propertyData->PropertyType == TypeOf_StringRef && !memcmp(valueData, "\0\0\0\0\0\0\0\0", sizeof(StringRef))) {
            *(StringRef*)valueData = nullStr;
        }
        memcpy(dataOut, valueData, size);
    }

    return true;
}


API_EXPORT void __InjectChildPropertyValue(Entity property, Entity context, Entity value) {
    Assert(property, IsEntityValid(value) && IsEntityValid(context));

    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);
    auto propertyData = (Property*) GetComponentBytes(ComponentOf_Property(), propertyIndex);

    if(!propertyData) {
        Log(context, LogSeverity_Error, "Property has not been registered.");
        return;
    }

    auto component = GetOwner(property);

    Assert(property, IsEntityValid(component));
    Assert(property, propertyData->PropertyKind == PropertyKind_Child);

    auto offset = propertyData->PropertyOffset;

    auto componentIndex = GetComponentIndex(component, context);
    Assert(property, componentIndex != InvalidIndex);
    auto componentData = GetComponentBytes(component, componentIndex);
    Assert(property, componentData);
    auto valueData = componentData + offset;

    *(Entity*)valueData = value;

    SetOwner(value, context, property);

    if(IsEntityValid(propertyData->PropertyChildComponent)) {
        AddComponent(value, propertyData->PropertyChildComponent);
    }
}

API_EXPORT u32 GetArrayPropertyCount(Entity property, Entity entity) {
    u32 count = 0;
    GetChildren(property, entity, &count);
    return count;
}

API_EXPORT bool SetArrayPropertyCount(Entity property, Entity entity, u32 count) {
    auto component = GetOwner(property);

    AddComponent(entity, component);

    auto existingCount = GetArrayPropertyCount(property, entity);

    if(count > existingCount) {
        for(auto i = existingCount; i < count; ++i) {
            AddArrayPropertyElement(property, entity);
        }
    } else if (count < existingCount) {
        while(existingCount > count) {
            RemoveArrayPropertyElement(property, entity, existingCount - 1);
            existingCount--;
        }
    }

    return true;
}


API_EXPORT u32 AddArrayPropertyElement(Entity property, Entity entity) {
    auto component = GetOwner(property);
    AddComponent(entity, component);

    auto element = CreateEntity();
    auto index = AddChild(property, entity, element, true);

    auto parentUuid = GetUuid(entity);
    auto propertyName = GetName(property);
    char *buffer = (char*)alloca(strlen(parentUuid) + strlen(propertyName) + 16);

    u32 i = index;
    bool success = false;
    do {
        sprintf(buffer, "%s.%s[%d]", parentUuid, propertyName, i);
        success = FindEntityByUuid(buffer) == 0;
        ++i;
    } while(!success);

    SetUuid(element, buffer);

    auto propertyData = GetPropertyData(property);
    AddComponent(element, propertyData->PropertyChildComponent);

    EmitChangedEvent(entity, property, propertyData, &nullEntity, &element);

    return index;
}

API_EXPORT bool RemoveArrayPropertyElement(Entity property, Entity entity, u32 index) {
    if(!HasComponent(property, ComponentOf_Property())) return false;

    u32 count = 0;
    auto children = GetChildren(property, entity, &count);

    if(index >= count) return false;

    auto child = children[index];

    EmitChangedEvent(entity, property, GetPropertyData(property), &child, &nullEntity);

    RemoveChild(property, entity, index);

    // Destroy removed entity if it is owned by the array carrier
    if(GetOwner(child) == entity) {
        DestroyEntity(entity);
    }

    return true;
}

API_EXPORT u32 GetArrayPropertyIndex(Entity property, Entity entity, Entity element) {
    return GetChildIndex(property, entity, element);
}

API_EXPORT Entity GetArrayPropertyElement(Entity property, Entity entity, u32 index) {
    u32 count = 0;
    auto children = GetChildren(property, entity, &count);
    if(!children) return 0;
    if(index >= count) return 0;

    return children[index];
}

API_EXPORT Entity *GetArrayPropertyElements(Entity property, Entity entity, u32 *count) {
    return GetChildren(property, entity, count);
}

void __Property(Entity property, u32 offset, u32 size, Type type, Entity component, Entity childComponent, u8 kind) {
    AddComponent(property, ComponentOf_Property());
    SetOwner(property, component, PropertyOf_Properties());
    auto data = GetPropertyData(property);
    data->PropertyOffset = offset;
    data->PropertySize = size;
    data->PropertyType = type;
    data->PropertyChildComponent = childComponent;
    data->PropertyKind = kind;
}

void __InitializeProperty() {
    auto component = ComponentOf_Property();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Property));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_PropertyOffset(), offsetof(Property, PropertyOffset), sizeof(Property::PropertyOffset), TypeOf_u32,  component, 0, PropertyKind_Value);
    __Property(PropertyOf_PropertySize(),   offsetof(Property, PropertySize),   sizeof(Property::PropertySize),   TypeOf_u32,  component, 0, PropertyKind_Value);
    __Property(PropertyOf_PropertyType(),   offsetof(Property, PropertyType),   sizeof(Property::PropertyType),   TypeOf_Type, component, 0, PropertyKind_Value);
    __Property(PropertyOf_PropertyKind(),   offsetof(Property, PropertyKind),   sizeof(Property::PropertyKind),   TypeOf_u8, component, 0, PropertyKind_Value);
    __Property(PropertyOf_PropertyChangedEvent(), offsetof(Property, PropertyChangedEvent), sizeof(Property::PropertyChangedEvent), TypeOf_Entity,  component, ComponentOf_Event(), PropertyKind_Child);

    component = ComponentOf_Ownership();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Ownership));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Owner(), offsetof(Ownership, Owner), sizeof(Ownership::Owner), TypeOf_Entity, component, 0, PropertyKind_Value);
    __Property(PropertyOf_OwnerProperty(), offsetof(Ownership, OwnerProperty), sizeof(Ownership::OwnerProperty), TypeOf_Entity, component, 0, PropertyKind_Value);
}

__PropertyCoreGet(Entity, Owner, Ownership)
__PropertyCoreGet(Entity, OwnerProperty, Ownership)

__PropertyCoreImpl(u32, PropertyOffset, Property)
__PropertyCoreImpl(u32, PropertySize, Property)
__PropertyCoreImpl(Type, PropertyType, Property)
__PropertyCoreImpl(Entity, PropertyEnum, Property)
__PropertyCoreImpl(Entity, PropertyChildComponent, Property)
__PropertyCoreImpl(Entity, PropertyChangedEvent, Property)
__PropertyCoreImpl(u8, PropertyKind, Property)
__PropertyCoreImpl(bool, PropertyReadOnly, Property)

API_EXPORT void SetOwner(Entity entity, Entity owner, Entity ownerProperty) {
    AddComponent(entity, ComponentOf_Ownership());
    auto data = GetOwnershipData(entity);

    auto oldOwner = data->Owner;
    auto oldOwnerProperty = data->OwnerProperty;

    data->Owner = owner;
    data->OwnerProperty = ownerProperty;

    EmitChangedEvent(entity, PropertyOf_Owner(), GetPropertyData(PropertyOf_Owner()), &oldOwner, &owner);
    EmitChangedEvent(entity, PropertyOf_OwnerProperty(), GetPropertyData(PropertyOf_OwnerProperty()), &oldOwnerProperty, &ownerProperty);
}

API_EXPORT void SetPropertyMeta(Entity property, StringRef metaString) {

}

static void LayoutProperties(Entity component) {
    auto offset = 0;

    for_children(property, Properties, component) {
        auto data = GetPropertyData(property);

        auto alignment = GetTypeAlignment(data->PropertyType);
        auto size = GetTypeSize(data->PropertyType);

        offset = Align(offset, alignment);

        if(data->PropertyType == TypeOf_Entity && data->PropertyKind == PropertyKind_Array) {
            size = sizeof(EntityVectorStruct) - 1 + size * data->PropertySize;
        }

        SetPropertyOffset(property, offset);

        offset += size;
    }

    offset = Align(offset, alignof(max_align_t)); // Align component size to maximum possible alignment

    if(GetComponentSize(component) != offset) {
        SetComponentSize(component, offset);
    }
}

LocalFunction(OnPropertyChanged, void, Entity property) {
    auto component = GetOwner(property);
    if(!GetComponentExplicitSize(component)) {
        LayoutProperties(component);
    }
}

LocalFunction(OnPropertyKindChanged, void, Entity property, u32 oldKind, u32 newKind) {
    auto propertyData = GetPropertyData(property);
    auto component = GetOwner(property);

    for_entity_abstract(entity, data, component, {
        if(oldKind == PropertyKind_Array) {
            EntityVectorStruct *vec = (EntityVectorStruct*)(data + propertyData->PropertyOffset);
            for(auto i = 0; i < vec->Count; ++i) {
                DestroyEntity(vec->StaBuf[i]);
                vec->StaBuf[i] = 0;
            }
            SetVectorAmount(*vec, 0);
        } else if(oldKind == PropertyKind_Child) {
            Entity *child = (Entity*)(data + propertyData->PropertyOffset);

            if(IsEntityValid(*child)) {
                DestroyEntity(*child);
            }
            *child = 0;
        }
    });

    if(!GetComponentExplicitSize(component)) {
        LayoutProperties(component);
    }

    for_entity_abstract(entity, data, component, {
        if(newKind == PropertyKind_Array) {
            EntityVectorStruct *vec = (EntityVectorStruct*)(data + propertyData->PropertyOffset);
            memset(vec, 0, sizeof(EntityVectorStruct));
        } else if(newKind == PropertyKind_Child) {
            Entity *child = (Entity*)(data + propertyData->PropertyOffset);

            *child = CreateEntity();
            SetOwner(*child, entity, property);
        } else {
            memset(data + propertyData->PropertyOffset, 0, GetTypeSize(propertyData->PropertyType));
        }
    });
}

LocalFunction(OnPropertyAdded, void, Entity unused, Entity property) {
    auto component = GetOwner(property);
    if(!GetComponentExplicitSize(component)) {
        LayoutProperties(component);
    }
}

LocalFunction(OnComponentExplicitSizeChanged, void, Entity component, bool oldValue, bool newValue) {
    if(newValue) {
        LayoutProperties(component);
    }
}

LocalFunction(OnPropertyOffsetChanged, void, Entity property, u32 oldValue, u32 newValue) {
    auto component = GetOwner(property);

    auto propertySize = GetTypeSize(GetPropertyType(property));
    if(GetPropertyKind(property) == PropertyKind_Array) {
        propertySize = sizeof(VectorStruct) - 1 + propertySize * GetPropertySize(property);
    }

    Entity entity = 0;
    char * data = 0;

    for_entity_abstract(entity, data, component, {
        memmove(data + newValue, data + oldValue, propertySize);
    });
}

BeginUnit(Property)
    BeginEnum(PropertyKind, false)
        RegisterFlag(PropertyKind_Value)
        RegisterFlag(PropertyKind_Child)
        RegisterFlag(PropertyKind_Array)
    EndEnum()
    BeginComponent(Property)
        RegisterProperty(u32, PropertyOffset)
        RegisterProperty(u32, PropertySize)
        RegisterProperty(Type, PropertyType)
        RegisterProperty(bool, PropertyReadOnly)
        RegisterPropertyEnum(u8, PropertyKind, PropertyKind)
        RegisterReferenceProperty(Enum, PropertyEnum)
        RegisterChildProperty(Event, PropertyChangedEvent)
        RegisterReferenceProperty(Component, PropertyChildComponent)
    EndComponent()
    BeginComponent(Ownership)
        RegisterProperty(Entity, Owner)
        RegisterReferenceProperty(Property, OwnerProperty)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnPropertyAdded, ComponentOf_Property())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PropertyKind()), OnPropertyKindChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PropertyType()), OnPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PropertySize()), OnPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PropertyOffset()), OnPropertyOffsetChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ComponentExplicitSize()), OnComponentExplicitSizeChanged, 0)
EndUnit()

static void DumpEntity(Entity entity, u32 indentLevel) {
    for(auto i = 0; i < indentLevel; ++i) printf("%s", "  ");
    printf("%s\n", GetUuid(entity));

    for_entity(child, data, Identification, {
        if(GetOwner(child) == entity) {
            DumpEntity(child, indentLevel+1);
        }
    });
}

API_EXPORT void DumpTree(Entity entity) {
    DumpEntity(entity, 0);
}

API_EXPORT void RegisterGenericPropertyChangedListener(GenericPropertyChangedListener listener) {
    PropertyChangedListeners.push_back(listener);
}

