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

#include <malloc.h>
#include <memory.h>
#include <cstring>

extern "C" {
    #include <ex_intern/strings.h>
    #include <ex_intern/optimize.h>
}

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
    bool PropertyReadOnly;
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

static struct strings* string_library = 0;
static struct strings_frequency* string_freq = 0;

API_EXPORT StringRef Intern(StringRef sourceString) {
    if(!sourceString) sourceString = "";

    auto id = strings_intern(string_library, sourceString);
    strings_frequency_add(string_freq, id);
    return strings_lookup_id(string_library, id);
}

static void FreeStrings() {
    if(string_library) {
        strings_free(string_library);
    }

    if(string_freq) {
        strings_frequency_free(string_freq);
    }
}

API_EXPORT void SetPropertyValue(Entity property, Entity context, const void *newValueData) {
    auto component = GetOwner(property);

    if(!IsEntityValid(component) || !IsEntityValid(property)) {
        Log(context, LogSeverity_Error, "Invalid property when trying to set property");
        return;
    }

    if(!IsEntityValid(context)) {
        Log(0, LogSeverity_Error, "Invalid entity when trying to set property");
        return;
    }

    static StringRef nullStr = Intern("");
    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    char *emptyPtr = nullData;
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

    AddComponent(context, component);

    auto offset = propertyData->PropertyOffset;
    auto size = GetTypeSize(propertyData->PropertyType);

    auto componentIndex = GetComponentIndex(component, context);
    auto componentData = GetComponentBytes(component, componentIndex);
    Assert(property, componentData);

    auto valueData = componentData + offset;
    char oldValueBuf[sizeof(m4x4f)];
    newValueData = newValueData ? newValueData : nullData;

    StringRef internedString = 0;
    if(propertyData->PropertyType == TypeOf_StringRef) {
        if(!memcmp(valueData, "\0\0\0\0\0\0\0\0", sizeof(StringRef))) {
            *(StringRef*)valueData = nullStr;
        }

        internedString = Intern(*(StringRef *) newValueData);
        newValueData = &internedString;
    }

    if(memcmp(valueData, newValueData, size) != 0) {
        memcpy(oldValueBuf, valueData, size);
        memcpy(valueData, newValueData, size);

        const Type argumentTypes[] = {TypeOf_Entity, propertyData->PropertyType, propertyData->PropertyType};
        const void * argumentData[] = {&context, oldValueBuf, newValueData};

        const Type genericArgumentTypes[] = {TypeOf_Entity, TypeOf_Entity};
        const void * genericArgumentData[] = {&property, &context};

        FireEventFast(propertyData->PropertyChangedEvent, 3, argumentTypes, argumentData);
        FireEventFast(EventOf_PropertyChanged(), 2, genericArgumentTypes, genericArgumentData);
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
        Log(0, LogSeverity_Error, "Invalid entity when trying to get property");
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

        Log(context, LogSeverity_Info, "'%s' is not present on entity. Returning default value.", GetDebugName(property), GetDebugName(component));

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


API_EXPORT u32 GetArrayPropertyCount(Entity property, Entity entity) {
    VectorStruct *s = 0;
    GetPropertyValue(property, entity, &s);

    return s ? s->Count : 0;
}

API_EXPORT bool SetArrayPropertyCount(Entity property, Entity entity, u32 count) {
    VectorStruct *s = 0;
    GetPropertyValue(property, entity, &s);

    if(!s) return false;

    if(count > s->Count) {
        for(auto i = s->Count; i < count; ++i) {
            AddArrayPropertyElement(property, entity);
        }
    } else if (count < s->Count) {
        while(s->Count > count) {
            RemoveArrayPropertyElement(property, entity, s->Count - 1);
        }
    }

    return true;
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

    SetOwner(element, entity, property);

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

    auto offset = propertyData->PropertyOffset;

    auto componentIndex = GetComponentIndex(component, context);
    auto componentData = GetComponentBytes(component, componentIndex);
    Assert(property, componentData);
    auto valueData = componentData + offset;

    *(Entity*)valueData = value;

    SetOwner(value, context, property);

    if(IsEntityValid(propertyData->PropertyChildComponent)) {
        AddComponent(value, propertyData->PropertyChildComponent);
    }

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
    if(!HasComponent(property, ComponentOf_Property())) return InvalidIndex;

    auto component = GetOwner(property);
    AddComponent(entity, component);

    auto element = __CreateEntity();
    auto propertyName = GetName(property);
    char name[512];
    auto index = GetEntityIndex(element);
    snprintf(name, 512, "%s_%u", propertyName, index);
    SetName(element, name);
    return __InjectArrayPropertyElement(property, entity, element);
}

API_EXPORT bool RemoveArrayPropertyElement(Entity property, Entity entity, u32 index) {
    if(!HasComponent(property, ComponentOf_Property())) return false;

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

API_EXPORT void CopyEntity(Entity templateEntity, Entity destinationEntity, bool removeRedundantComponents) {
	char buffer[128];
	Entity source, destination;

	if(!IsEntityValid(templateEntity)) {
	    Log(0, LogSeverity_Error, "Cannot copy from invalid entity", GetDebugName(templateEntity));
	    return;
	}

    if(!IsEntityValid(templateEntity)) {
        Log(0, LogSeverity_Error, "Cannot copy to invalid entity", GetDebugName(destinationEntity));
        return;
    }

    for_entity(component, data, Component) {
        if (component != ComponentOf_Ownership() && component != ComponentOf_Identification() && HasComponent(templateEntity, component)) {
            for_children(property, Properties, component) {
                switch (GetPropertyKind(property)) {
                    case PropertyKind_Value:
                        GetPropertyValue(property, templateEntity, buffer);
                        SetPropertyValue(property, destinationEntity, buffer);
                        break;
                    case PropertyKind_Child:
                        GetPropertyValue(property, templateEntity, &source);
                        GetPropertyValue(property, destinationEntity, &destination);
                        if(IsEntityValid(source) && IsEntityValid(destination)) {
                            CopyEntity(source, destination);
                        }
                        break;
                    case PropertyKind_Array:
                        SetArrayPropertyCount(property, destinationEntity, GetArrayPropertyCount(property, templateEntity));

                        auto sourceElements = GetArrayPropertyElements(property, templateEntity);
                        auto destinationElements = GetArrayPropertyElements(property, destinationEntity);
                        for(auto i = 0; i < GetArrayPropertyCount(property, templateEntity); ++i) {
                            CopyEntity(sourceElements[i], destinationElements[i]);
                        }
                }
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

void __InitializeString() {
    string_library = strings_new();
    string_freq = strings_frequency_new();

    atexit(FreeStrings);
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

__PropertyCoreImpl(u32, PropertyOffset, Property)
__PropertyCoreImpl(u32, PropertySize, Property)
__PropertyCoreImpl(Type, PropertyType, Property)
__PropertyCoreImpl(Entity, PropertyEnum, Property)
__PropertyCoreImpl(Entity, PropertyChildComponent, Property)
__PropertyCoreImpl(Entity, PropertyChangedEvent, Property)
__PropertyCoreImpl(u8, PropertyKind, Property)
__PropertyCoreImpl(bool, PropertyReadOnly, Property)

API_EXPORT void SetOwner(Entity entity, Entity owner, Entity ownerProperty) {
    Assert(entity, IsEntityValid(owner));
    Assert(entity, IsEntityValid(ownerProperty));

    AddComponent(entity, ComponentOf_Ownership());
    auto data = GetOwnershipData(entity);

    auto oldOwner = data->Owner;
    data->Owner = owner;
    data->OwnerProperty = ownerProperty;

    const Type argumentTypes[] = {TypeOf_Entity, TypeOf_Entity, TypeOf_Entity};
    const void * argumentData[] = {&entity, &oldOwner, &owner};

    FireEventFast(EventOf_OwnerChanged(), 3, argumentTypes, argumentData);
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
    for(auto i = GetNextComponent(component, InvalidIndex, (void**)&data, &entity);
        i != InvalidIndex;
        i = GetNextComponent(component, i, (void**)&data, &entity)) {
        memmove(data + newValue, data + oldValue, propertySize);
    }
}

BeginUnit(Property)
    RegisterEvent(PropertyChanged)
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
    BeginComponent(Binding)
        RegisterProperty(Entity, BindingSourceEntity)
        RegisterReferenceProperty(Property, BindingSourceProperty)
        RegisterProperty(Entity, BindingTargetEntity)
        RegisterReferenceProperty(Property, BindingTargetProperty)
    EndComponent()
    BeginComponent(Ownership)
        RegisterProperty(Entity, Owner)
        RegisterReferenceProperty(Property, OwnerProperty)
    EndComponent()

    RegisterSubscription(EntityComponentAdded, OnPropertyAdded, ComponentOf_Property())
    RegisterSubscription(PropertyTypeChanged, OnPropertyChanged, 0)
    RegisterSubscription(PropertySizeChanged, OnPropertyChanged, 0)
    RegisterSubscription(PropertyOffsetChanged, OnPropertyOffsetChanged, 0)
    RegisterSubscription(ComponentExplicitSizeChanged, OnComponentExplicitSizeChanged, 0)
EndUnit()

