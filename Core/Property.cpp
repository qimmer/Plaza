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

#include <EASTL/map.h>

#include <malloc.h>
#include <memory.h>
#include <cstring>

#define Verbose_Children "children"
#define Verbose_PropertyChanges "changes"

static Entity nullEntity = 0;

struct EntityVectorStruct {
	u32 Count;
	u32 DynCapacity;
	Entity *DynBuf;
	Entity StaBuf[1];
};

struct PropertyChildArray {
    eastl::map<Entity, ChildArray> PerEntityChildrenInfo;
};

static eastl::map<Entity, PropertyChildArray> PerPropertyChildrenInfo;
static Vector<GenericPropertyChangedListener, 64> PropertyChangedListeners;

static inline PropertyChildArray& GetPropertyChildren(Entity property) {
    return PerPropertyChildrenInfo[property];
}

API_EXPORT Vector<GenericPropertyChangedListener, 64>& GetGenericPropertyChangedListeners() {
    return PropertyChangedListeners;
}

API_EXPORT u32 AddChild(Entity property, Entity entity, Entity child, bool takeOwnership) {
    if(!IsEntityValid(child)) {
        Error(entity, "Cannot add invalid %s child", GetDebugName(property));
        return InvalidIndex;
    }

    auto& propertyChildren = GetPropertyChildren(property);

    Verbose(Verbose_Children, "Added %s '%s' to '%s'", GetDebugName(property), GetDebugName(child), GetDebugName(entity));

    auto owner = GetOwner(child);
    auto ownerProperty = GetOwnerProperty(child);

    // If child has current owner, remove child from owner, as we are taking over ownership of this child
    if(takeOwnership && owner != entity && IsEntityValid(owner)) {
        auto index = GetChildIndex(ownerProperty, owner, child);
        RemoveChild(ownerProperty, owner, index);
    }

    auto& info = propertyChildren.PerEntityChildrenInfo[entity];

    // Insert child into end of child array, pushing all subsequent entries one index
    //propertyChildren->ChildrenPool.insert(propertyChildren->ChildrenPool.begin() + info.StartIndex + info.Count, child);

    info.push_back(child);

    if(takeOwnership) {
        SetOwner(child, entity, property);
    }

    return info.size() - 1;
}

API_EXPORT bool RemoveChild(Entity property, Entity entity, u32 index) {
    auto& propertyChildren = GetPropertyChildren(property);

    auto& info = propertyChildren.PerEntityChildrenInfo[entity];
    if(index >= info.size()) return false;

    auto child = info[index];//propertyChildren->ChildrenPool[info.StartIndex + index];

    Verbose(Verbose_Children, "Removed %s '%s' to '%s'", GetDebugName(property), GetDebugName(child), GetDebugName(entity));

    if(GetOwner(child) == entity) {
        SetOwner(child, 0, 0);
    }

    info.erase(info.begin()  + index);

    return true;
}

static const eastl::vector<Entity> emptyArray;

API_EXPORT u32 GetChildIndex(Entity property, Entity entity, Entity child) {
    auto& children = GetChildArray(property, entity);

    for(u32 i = 0; i < children.size(); ++i) {
        if(children[i] == child) return i;
    }

    return InvalidIndex;
}

API_EXPORT const ChildArray& GetChildArray(Entity property, Entity entity) {
    auto& propertyChildren = GetPropertyChildren(property);

    auto& info = propertyChildren.PerEntityChildrenInfo[entity];
    return info;
}

API_EXPORT void EmitChangedEvent(Entity entity, Entity property, Property *propertyData, Variant oldValueData, Variant newValueData) {
    if(!propertyData) return;

    auto& cache = GetSubscriptionCache(propertyData->PropertyChangedEvent);

    Variant arguments[] = { MakeVariant(Entity, entity), oldValueData, newValueData};

    for(auto i = 0; i < cache.size(); ++i) {
        auto& subscription = cache[i];
        if(!subscription.SubscriptionSender || subscription.SubscriptionSender == entity) {
            auto functionData = GetFunctionData(subscription.SubscriptionHandler);

            ((FunctionCallerType)functionData->FunctionCaller)(
                    functionData->FunctionPtr,
                    arguments
            );
        }
    }

    for(auto listener : PropertyChangedListeners) {
        listener(property, entity, propertyData->PropertyType, oldValueData, newValueData);
    }
}

API_EXPORT void SetPropertyValue(Entity property, Entity context, Variant newValueData) {
	auto propertyData = GetPropertyData(property);
	if(!propertyData) {
        Log(context, LogSeverity_Error, "Invalid property when trying to set property %s on %s", GetDebugName(property), GetDebugName(context));
        return;
	}

    u32 offset = propertyData->PropertyOffset;
    Entity component = GetOwner(property);
    auto size = GetTypeSize(propertyData->PropertyType);
	auto isString = propertyData->PropertyType == TypeOf_StringRef;
	auto isVariant = propertyData->PropertyType == TypeOf_Variant;

	if(propertyData->PropertyType != newValueData.type) {
	    newValueData = Cast(newValueData, propertyData->PropertyType);
	}
        
    auto componentIndex = GetComponentIndex(component, context);
        
    if(componentIndex == InvalidIndex) {
        AddComponent(context, component);
        componentIndex = GetComponentIndex(component, context);
    }
        
    auto componentData = GetComponentData(component, componentIndex);
    auto valueData = componentData + offset;

    if(propertyData->PropertyType == TypeOf_StringRef || (propertyData->PropertyType == TypeOf_Variant && ((Variant*)valueData)->type == TypeOf_StringRef)) {
        ReleaseStringRef(*(StringRef*)valueData);
    }

    if(propertyData->PropertyType == TypeOf_StringRef || (propertyData->PropertyType == TypeOf_Variant && newValueData.type == TypeOf_StringRef)) {
        newValueData.as_StringRef = AddStringRef(newValueData.as_StringRef);
    }

    if(propertyData->PropertyType == TypeOf_Variant) {
        if(memcmp(valueData, &newValueData, size) != 0) {
            Variant oldValueData;
            memcpy(&oldValueData, valueData, size);
            memcpy(valueData, &newValueData, size);

            EmitChangedEvent(context, property, propertyData, oldValueData, newValueData);
        }
    } else {
        if(memcmp(valueData, &newValueData.data, size) != 0) {
            Variant oldValueData;
            oldValueData.type = propertyData->PropertyType;
            memcpy(&oldValueData.data, valueData, size);
            memcpy(valueData, &newValueData.data, size);

            EmitChangedEvent(context, property, propertyData, oldValueData, newValueData);
        }
    }

}

API_EXPORT Variant GetPropertyValue(Entity property, Entity context) {
    auto component = GetOwner(property);

    if(!IsEntityValid(property)) {
        Log(context, LogSeverity_Error, "Invalid property when trying to get property");
        return Variant_Empty;
    }

    if(!IsEntityValid(component)) {
        Log(context, LogSeverity_Error, "Invalid component when trying to get property %s", GetDebugName(property));
        return Variant_Empty;
    }

    if(!IsEntityValid(context)) {
        Log(0, LogSeverity_Error, "Invalid entity when trying to get property %s", GetDebugName(property));
        return Variant_Empty;
    }

    auto propertyData = GetPropertyData(property);

    auto componentIndex = GetComponentIndex(component, context);
    if(componentIndex == InvalidIndex) return Variant_Default;
    auto componentData = GetComponentData(component, componentIndex);

    auto valueData = componentData + propertyData->PropertyOffset;
	
    return __MakeVariant(valueData, propertyData->PropertyType);
}


API_EXPORT u32 GetArrayPropertyCount(Entity property, Entity entity) {
    return GetChildArray(property, entity).size();
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
    auto propertyName = strrchr(GetUuid(property), '.') + 1;
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

    EmitChangedEvent(entity, property, propertyData, MakeVariant(Entity, nullEntity), MakeVariant(Entity, element));

    return index;
}

API_EXPORT bool RemoveArrayPropertyElement(Entity property, Entity entity, u32 index) {
    if(!HasComponent(property, ComponentOf_Property())) return false;

    auto& children = GetChildArray(property, entity);

	if(index >= children.size()) return false;

    auto child = children[index];

    // Destroy removed entity if it is owned by the array carrier
    if(GetOwner(child) == entity) {
        DestroyEntity(child);
    }

    EmitChangedEvent(entity, property, GetPropertyData(property), MakeVariant(Entity, child), MakeVariant(Entity, nullEntity));

    RemoveChild(property, entity, index);

    return true;
}

API_EXPORT u32 GetArrayPropertyIndex(Entity property, Entity entity, Entity element) {
    return GetChildIndex(property, entity, element);
}

API_EXPORT Entity GetArrayPropertyElement(Entity property, Entity entity, u32 index) {
    auto& children = GetChildArray(property, entity);
    if(index >= children.size()) return 0;

    return children[index];
}

API_EXPORT const ChildArray& GetArrayPropertyElements(Entity property, Entity entity) {
    return GetChildArray(property, entity);
}

void __Property(Entity property, u32 offset, u32 size, Type type, Entity component, Entity childComponent, u8 kind, StringRef name) {
    AddComponent(property, ComponentOf_Property());
    SetOwner(property, component, PropertyOf_Properties());
	SetUuid(property, StringFormatV("Property.%s", name));
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
	SetUuid(component, "Component.Property");
    __Property(PropertyOf_PropertyOffset(), offsetof(Property, PropertyOffset), sizeof(Property::PropertyOffset), TypeOf_u32,  component, 0, PropertyKind_Value, "PropertyOffset");
    __Property(PropertyOf_PropertySize(),   offsetof(Property, PropertySize),   sizeof(Property::PropertySize),   TypeOf_u32,  component, 0, PropertyKind_Value, "PropertySize");
    __Property(PropertyOf_PropertyType(),   offsetof(Property, PropertyType),   sizeof(Property::PropertyType),   TypeOf_Type, component, 0, PropertyKind_Value, "PropertyType");
    __Property(PropertyOf_PropertyKind(),   offsetof(Property, PropertyKind),   sizeof(Property::PropertyKind),   TypeOf_u8, component, 0, PropertyKind_Value, "PropertyKind");
    __Property(PropertyOf_PropertyChangedEvent(), offsetof(Property, PropertyChangedEvent), sizeof(Property::PropertyChangedEvent), TypeOf_Entity,  component, ComponentOf_Event(), PropertyKind_Child, "PropertyChangedEvent");

    component = ComponentOf_Ownership();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Ownership));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
	SetUuid(component, "Component.Ownership");
    __Property(PropertyOf_Owner(), offsetof(Ownership, Owner), sizeof(Ownership::Owner), TypeOf_Entity, component, 0, PropertyKind_Value, "Owner");
    __Property(PropertyOf_OwnerProperty(), offsetof(Ownership, OwnerProperty), sizeof(Ownership::OwnerProperty), TypeOf_Entity, component, 0, PropertyKind_Value, "OwnerProperty");
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

__PropertyCoreImpl(StringRef, Name, ArrayChild)

API_EXPORT void SetOwner(Entity entity, Entity owner, Entity ownerProperty) {
    AddComponent(entity, ComponentOf_Ownership());
    auto data = GetOwnershipData(entity);

    auto oldOwner = MakeVariant(Entity, data->Owner);
    auto oldOwnerProperty = MakeVariant(Entity, data->OwnerProperty);

    data->Owner = owner;
    data->OwnerProperty = ownerProperty;

    EmitChangedEvent(entity, PropertyOf_Owner(), GetPropertyData(PropertyOf_Owner()), oldOwner, MakeVariant(Entity, owner));
    EmitChangedEvent(entity, PropertyOf_OwnerProperty(), GetPropertyData(PropertyOf_OwnerProperty()), oldOwnerProperty, MakeVariant(Entity, ownerProperty));
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

    for_entity_abstract(entity, data, component) {
        if(oldKind == PropertyKind_Array) {
            EntityVectorStruct *vec = (EntityVectorStruct*)(data + propertyData->PropertyOffset);
            for(u32 i = 0; i < vec->Count; ++i) {
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
    }

    if(!GetComponentExplicitSize(component)) {
        LayoutProperties(component);
    }

    {
        for_entity_abstract(entity, data, component) {
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
        }
    }
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

    for_entity_abstract(entity, data, component) {
        memmove(data + newValue, data + oldValue, propertySize);
    }
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
    BeginComponent(ArrayChild)
        //RegisterProperty(StringRef, Name)
    EndComponent()

    RegisterFunction(SetPropertyValue)
    RegisterFunction(GetPropertyValue)

    RegisterSubscription(EventOf_EntityComponentAdded(), OnPropertyAdded, ComponentOf_Property())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PropertyKind()), OnPropertyKindChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PropertyType()), OnPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PropertySize()), OnPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_PropertyOffset()), OnPropertyOffsetChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ComponentExplicitSize()), OnComponentExplicitSizeChanged, 0)
EndUnit()

static void DumpEntity(Entity entity, u32 indentLevel) {
    for(u32 i = 0; i < indentLevel; ++i) printf("%s", "  ");
    printf("%s\n", GetUuid(entity));

    for_entity(child, data, Identification) {
        if(GetOwner(child) == entity) {
            DumpEntity(child, indentLevel+1);
        }
    }
}

API_EXPORT void DumpTree(Entity entity) {
    DumpEntity(entity, 0);
}

API_EXPORT void RegisterGenericPropertyChangedListener(GenericPropertyChangedListener listener) {
    PropertyChangedListeners.push_back(listener);
}

API_EXPORT bool IsOwnedBy(Entity entity, Entity owner) {
    auto entityOwner = GetOwner(entity);
    return IsEntityValid(entityOwner) && (entityOwner == owner || IsOwnedBy(entityOwner, owner));
}
