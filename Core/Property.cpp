//
// Created by Kim Johannsen on 02/01/2018.
//

#include <Core/Property.h>
#include <Core/Component.h>
#include <malloc.h>
#include <memory.h>
#include <cstring>
#include "Function.h"
#include "Debug.h"
#include "Entity.h"
#include "Vector.h"
#include "Enum.h"
#include "Identification.h"

static Entity nullEntity = 0;

struct EntityVectorStruct {
	u32 Count;
	u32 DynCapacity;
	Entity *DynBuf;
	Entity StaBuf[1];
};

struct Ownership {
    Entity Owner, OwnerProperty;
};

struct Property {
    Entity PropertyEnum, PropertyChangedEvent, PropertyChildComponent;
    u32 PropertyOffset, PropertySize, PropertyFlags;
    Type PropertyType;
    u8 PropertyKind;
};

struct Binding {
    Entity BindingSourceEntity, BindingSourceProperty, BindingTargetEntity, BindingTargetProperty;
};

struct ChildProperty {
    Entity ChildPropertyComponent;
};

struct ArrayProperty {
    Entity ArrayPropertyComponent;
};

API_EXPORT void SetPropertyValue(Entity property, Entity context, const void *newValueData) {
    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);
    auto propertyData = (Property*) GetComponentBytes(ComponentOf_Property(), propertyIndex);

    if(!propertyData) {
        Log(context, LogSeverity_Error, "Property has not been registered.");
        return;
    }

    if(propertyData->PropertyKind == PropertyKind_Array || propertyData->PropertyKind == PropertyKind_Child) {
        Log(context, LogSeverity_Error, "Array and child properties cannot be directly set. Child properties are fixed and set on component creation and array properties must use Add and Remove.");
        return;
    }

    auto component = GetOwner(property);

    Assert(property, IsEntityValid(component));
    AddComponent(context, component);

    auto offset = propertyData->PropertyOffset;

    auto componentIndex = GetComponentIndex(component, context);
    auto componentData = GetComponentBytes(component, componentIndex);
    Assert(property, componentData);

    auto valueData = componentData + offset;

    char *oldValueData = (char*)alloca(propertyData->PropertySize);

    bool changed = false;
    if(propertyData->PropertyKind == PropertyKind_String) {
        strncpy(oldValueData, valueData, propertyData->PropertySize);
        if(strcmp(oldValueData, *(const char**)newValueData) != 0) {
            strcpy(valueData, *(const char**)newValueData);
            changed = true;
        }
    } else {
        memcpy(oldValueData, valueData, propertyData->PropertySize);
        if(memcmp(oldValueData, newValueData, propertyData->PropertySize) != 0) {
            memcpy(valueData, newValueData, propertyData->PropertySize);
            changed = true;
        }
    }

    if(changed) {
        const Type argumentTypes[] = {TypeOf_Entity, propertyData->PropertyType, propertyData->PropertyType};
        const void * argumentData[] = {&context, oldValueData, newValueData};

        const Type genericArgumentTypes[] = {TypeOf_Entity, TypeOf_Entity};
        const void * genericArgumentData[] = {&property, &context};

        // If property data is a char array, change valueData to point to a char pointer (StringRef) instead of the actual char data.
        if(propertyData->PropertyKind == PropertyKind_String) {
            argumentData[1] = (char *) &newValueData;
        }

        FireEventFast(propertyData->PropertyChangedEvent, 3, argumentTypes, argumentData);
        FireEventFast(EventOf_PropertyChanged(), 2, genericArgumentTypes, genericArgumentData);
    }
}

API_EXPORT bool GetPropertyValue(Entity property, Entity context, void *dataOut) {
    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);
    auto propertyData = (Property*) GetComponentBytes(ComponentOf_Property(), propertyIndex);

    if(!propertyData) {
        return false;
    }

    auto component = GetOwner(property);

    auto offset = propertyData->PropertyOffset;

    auto componentIndex = GetComponentIndex(component, context);
    auto componentData = GetComponentBytes(component, componentIndex);

    if(componentIndex == InvalidIndex || !componentData) {
        if(propertyData->PropertyKind == PropertyKind_String) {
            *(const char**)dataOut = "";
        } else if(propertyData->PropertyKind == PropertyKind_Array) {
            *(VectorStruct**)dataOut = 0;
        } else {
            memset(dataOut, 0, GetTypeSize(propertyData->PropertyType));
        }

        return false;
    }

    auto valueData = componentData + offset;

    if(propertyData->PropertyKind == PropertyKind_String) {
        *(const char**)dataOut = valueData;
    } else if(propertyData->PropertyKind == PropertyKind_Array) {
        *(VectorStruct**)dataOut = (VectorStruct*)valueData;
    } else {
        memcpy(dataOut, valueData, propertyData->PropertySize);
    }

    return true;
}


API_EXPORT u32 GetArrayPropertyCount(Entity property, Entity entity) {
    VectorStruct *s = 0;
    GetPropertyValue(property, entity, &s);

    return s ? s->Count : 0;
}

API_EXPORT u32 __InjectArrayPropertyElement(Entity property, Entity entity, Entity element) {
    Assert(property, IsEntityValid(element));

    auto propertyData = GetPropertyData(property);

	EntityVectorStruct *s = 0;
    GetPropertyValue(property, entity, &s);
    Assert(entity, s);

    auto elementSize = GetTypeSize(propertyData->PropertyType);
    auto staticCap = (propertyData->PropertySize - sizeof(u32) * 2 - sizeof(char*)) / elementSize;
    __SetVectorAmount(&s->Count, s->Count + 1, &s->DynCapacity, staticCap, (void**)&s->DynBuf, s->StaBuf, elementSize);

	{
		auto elementData = &(GetVector(*s)[s->Count - 1]);

		*(Entity*)elementData = element;
	}
    
    AddComponent(element, ComponentOf_Ownership());

    auto ownership = GetOwnershipData(element);
    ownership->Owner = entity;
    ownership->OwnerProperty = property;

    AddComponent(element, propertyData->PropertyChildComponent);

	{
		const Type argumentTypes[] = { TypeOf_Entity, TypeOf_Entity, TypeOf_Entity };
		const void * argumentData[] = { &entity, &nullEntity, &element };

		const Type genericArgumentTypes[] = { TypeOf_Entity, TypeOf_Entity };
		const void * genericArgumentData[] = { &property, &entity };

		FireEventFast(propertyData->PropertyChangedEvent, 3, argumentTypes, argumentData);
		FireEventFast(EventOf_PropertyChanged(), 2, genericArgumentTypes, genericArgumentData);
	}
    
    return s->Count - 1;
}


API_EXPORT void __InjectChildPropertyValue(Entity property, Entity context, Entity value) {
    Assert(property, IsEntityValid(value));

    auto propertyIndex = GetComponentIndex(ComponentOf_Property(), property);
    auto propertyData = (Property*) GetComponentBytes(ComponentOf_Property(), propertyIndex);

    if(!propertyData) {
        Log(context, LogSeverity_Error, "Property has not been registered.");
        return;
    }

    auto component = GetOwner(property);

    Assert(property, IsEntityValid(component));
    Assert(property, propertyData->PropertyKind == PropertyKind_Child);
    AddComponent(context, component);

    auto offset = propertyData->PropertyOffset;

    auto componentIndex = GetComponentIndex(component, context);
    auto componentData = GetComponentBytes(component, componentIndex);
    Assert(property, componentData);
    auto valueData = componentData + offset;

    *(Entity*)valueData = value;

    SetOwner(value, context, property);

    AddComponent(value, propertyData->PropertyChildComponent);

    {
        const Type argumentTypes[] = { TypeOf_Entity, TypeOf_Entity, TypeOf_Entity };
        const void * argumentData[] = { &context, &nullEntity, &value };

        const Type genericArgumentTypes[] = { TypeOf_Entity, TypeOf_Entity };
        const void * genericArgumentData[] = { &property, &context };

        FireEventFast(propertyData->PropertyChangedEvent, 3, argumentTypes, argumentData);
        FireEventFast(EventOf_PropertyChanged(), 2, genericArgumentTypes, genericArgumentData);
    }
}

API_EXPORT u32 AddArrayPropertyElement(Entity property, Entity entity) {
    auto element = __CreateEntity();
    auto propertyName = GetName(property);
    char name[512];
    snprintf(name, 512, "%s_%lu", propertyName, GetEntityIndex(element));
    SetName(element, name);
    return __InjectArrayPropertyElement(property, entity, element);
}

API_EXPORT bool RemoveArrayPropertyElement(Entity property, Entity entity, u32 index) {
    auto propertyData = GetPropertyData(property);

	EntityVectorStruct *s = 0;
    GetPropertyValue(property, entity, &s);

    if(!s) return false;

    auto removalElementData = &GetVector(*s)[index];
    auto lastElementData = &GetVector(*s)[s->Count - 1];

    auto removalEntity = *(Entity*)removalElementData;

    const Type argumentTypes[] = {TypeOf_Entity, TypeOf_Entity, TypeOf_Entity};
    const void * argumentData[] = {&entity, &removalEntity, &nullEntity};

    const Type genericArgumentTypes[] = {TypeOf_Entity, TypeOf_Entity};
    const void * genericArgumentData[] = {&property, &entity};

    FireEventFast(propertyData->PropertyChangedEvent, 3, argumentTypes, argumentData);
    FireEventFast(EventOf_PropertyChanged(), 2, genericArgumentTypes, genericArgumentData);

    // Destroy removed entity if it is owned by the array carrier
    if(GetOwner(removalEntity) == entity) {
        __DestroyEntity(removalEntity);
    }

    // First, replace removal element with last element
	if (removalElementData != lastElementData) {
		memmove(removalElementData, lastElementData, sizeof(Entity));
	}

    // Now, cut the last element off from the end, shrinking the array
	GetVector(*s)[index] = GetVector(*s)[s->Count - 1];
	GetVector(*s)[s->Count - 1] = 0;
	
	auto elementSize = GetTypeSize(propertyData->PropertyType);
	auto staticCap = (propertyData->PropertySize - sizeof(u32) * 2 - sizeof(char*)) / elementSize;
	__SetVectorAmount(&s->Count, s->Count - 1, &s->DynCapacity, staticCap, (void**)&s->DynBuf, s->StaBuf, elementSize);

    return true;
}

API_EXPORT Entity GetArrayPropertyElement(Entity property, Entity entity, u32 index) {
    auto propertyData = GetPropertyData(property);

	EntityVectorStruct *s = 0;
    GetPropertyValue(property, entity, &s);

    if(!s) return 0;

    Assert(entity, s && s->Count > index);
    auto child = (GetVector(*s))[index];
    return child;
}

API_EXPORT Entity *GetArrayPropertyElements(Entity property, Entity entity) {
    auto propertyData = GetPropertyData(property);

    EntityVectorStruct *s = 0;
    GetPropertyValue(property, entity, &s);

    return s ? (GetVector(*s)) : NULL;
}

API_EXPORT void CopyEntity(Entity templateEntity, Entity destinationEntity) {
	char buffer[128];

	for (auto i = 0; i < GetComponentMax(ComponentOf_Component()); ++i) {
		auto component = GetComponentEntity(ComponentOf_Component(), i);

		if (HasComponent(templateEntity, component)) {
			for (auto j = 0; j < GetComponentMax(ComponentOf_Property()); ++j) {
				auto property = GetComponentEntity(ComponentOf_Property(), j);

				if (GetOwner(property) != component) continue;

				GetPropertyValue(property, templateEntity, buffer);
				SetPropertyValue(property, destinationEntity, buffer);
			}
		}
	}
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
}

__PropertyCoreGet(Entity, Owner, Ownership)
__PropertyCoreGet(Entity, OwnerProperty, Ownership)

API_EXPORT void SetOwner(Entity entity, Entity owner, Entity ownerProperty) {
    Assert(entity, IsEntityValid(owner));
    Assert(entity, IsEntityValid(ownerProperty));

    AddComponent(entity, ComponentOf_Ownership());
    auto data = GetOwnershipData(entity);
    data->Owner = owner;
    data->OwnerProperty = ownerProperty;
}

__PropertyCoreImpl(u32, PropertyOffset, Property)
__PropertyCoreImpl(u32, PropertySize, Property)
__PropertyCoreImpl(Type, PropertyType, Property)
__PropertyCoreImpl(Entity, PropertyEnum, Property)
__PropertyCoreImpl(u32, PropertyFlags, Property)
__PropertyCoreImpl(Entity, PropertyChildComponent, Property)
__PropertyCoreImpl(u8, PropertyKind, Property)

API_EXPORT void SetPropertyMeta(Entity property, StringRef metaString) {

}

BeginUnit(Property)
    RegisterEvent(PropertyChanged)
    BeginComponent(Property)
        RegisterProperty(u32, PropertyOffset)
        RegisterProperty(u32, PropertySize)
        RegisterProperty(Type, PropertyType)
        RegisterProperty(u32, PropertyFlags)
        RegisterProperty(u8, PropertyKind)
        RegisterProperty(Entity, PropertyEnum)
        RegisterChildProperty(Event, PropertyChangedEvent)
        RegisterProperty(Entity, PropertyChildComponent)
    EndComponent()
    BeginComponent(Binding)
        RegisterProperty(Entity, BindingSourceEntity)
        RegisterProperty(Entity, BindingSourceProperty)
        RegisterProperty(Entity, BindingTargetEntity)
        RegisterProperty(Entity, BindingTargetProperty)
    EndComponent()
    BeginComponent(Ownership)
        RegisterProperty(Entity, Owner)
        RegisterProperty(Entity, OwnerProperty)
    EndComponent()
EndUnit()

