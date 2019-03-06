//
// Created by Kim on 03/06/2018.
//

#include <Core/NativeUtils.h>
#include "Identification.h"
#include "Component.h"


static bool __isComponentInitialized = false;

#define Verbose_Component "component"

static ComponentTypeData ComponentDataList[SHRT_MAX];

struct Extension {
    Entity ExtensionComponent, ExtensionExtenderComponent;
    bool ExtensionDisabled;
};

struct Base {
    Entity BaseComponent;
};

struct Component {
    u16 ComponentDataIndex;
    u16 ComponentSize;
    bool ComponentExplicitSize;
};

API_EXPORT ComponentTypeData* GetComponentType(u16 componentEntityIndex) {
	return &ComponentDataList[componentEntityIndex];
}

API_EXPORT bool HasComponent (Entity entity, Entity component) {
    auto componentData = GetComponentType(GetEntityIndex(component));
    auto count = componentData->EntityComponentIndices.Count;
    auto entityIndex = GetEntityIndex(entity);

    if (count <= entityIndex) {
        return false;
    }

    return GetVector(componentData->EntityComponentIndices)[entityIndex] != InvalidIndex;
}

API_EXPORT bool AddComponent (Entity entity, Entity component) {
    if(!HasComponent (entity, component)) {
        if(!IsEntityValid(entity)) {
            Log(0, LogSeverity_Error, "Invalid Entity when adding component %s: %s", GetDebugName(component), GetDebugName(entity));
            return false;
        }

        if(!IsEntityValid(component)) {
            Log(0, LogSeverity_Error, "Cannot add invalid Component");
            return false;
        }

        auto componentData = GetComponentType(GetEntityIndex(component));

        auto componentIndex = componentData->DataBuffer.Add();
        auto entityIndex = GetEntityIndex(entity);

        auto count = componentData->EntityComponentIndices.Count;
        if(count <= entityIndex) {
            auto newCount = entityIndex + 1;
            SetVectorAmount(componentData->EntityComponentIndices, newCount);
            memset(&GetVector(componentData->EntityComponentIndices)[count], 0xff, sizeof(u32) * (newCount - count));
        }

        GetVector(componentData->EntityComponentIndices)[entityIndex] = componentIndex;
        *(Entity*)componentData->DataBuffer[componentIndex] = entity;

        auto dataSize = GetComponentSize(component);
        if(dataSize > 0) {
            memset(componentData->DataBuffer[componentIndex] + sizeof(Entity)*2, 0, dataSize);
        }

        if(__isComponentInitialized) {
            Variant arguments[] = {MakeVariant(Entity, component), MakeVariant(Entity, entity)};

            auto parentUuid = GetUuid(entity);
            char buffer[1024];

            // Initialize children
            for_children(property, Properties, component) {
                auto propertyData = GetPropertyData(property);
                if(propertyData->PropertyKind != PropertyKind_Child) continue;

                auto child = CreateEntity();
                auto propertyName = strrchr(GetUuid(property), '.') + 1;

                snprintf(buffer, sizeof(buffer), "%s.%s", parentUuid, propertyName);

                SetUuid(child, buffer);

                auto offset = propertyData->PropertyOffset;
                *(Entity*)(componentData->DataBuffer[componentIndex] + sizeof(Entity)*2 + offset) = child;

                SetOwner(child, entity, property);

                EmitChangedEvent(entity, property, propertyData, Variant_Default, MakeVariant(Entity, child));

                AddComponent(child, propertyData->PropertyChildComponent);
            }

            FireEventFast(EventOf_EntityComponentAdding(), 2, arguments);

            for_children(base, Bases, component) {
                AddComponent(entity, GetBaseComponent(base));
            }

            FireEventFast(EventOf_EntityComponentAdded(), 2, arguments);

            for_entity(extension, extensionData, Extension) {
                if(extensionData->ExtensionComponent == component) {
                    if(!extensionData->ExtensionDisabled) {
                        AddComponent(entity, extensionData->ExtensionExtenderComponent);
                    }
                }
            }
        }

        Verbose(Verbose_Component, "Component %s has been added to Entity %s.", GetDebugName(component), GetDebugName(entity));

        return true;
    }
    return false;
}

API_EXPORT bool RemoveComponent (Entity entity, Entity component) {
    if(!IsEntityValid(entity)) {
        Log(0, LogSeverity_Error, "Invalid Entity when removing component %s: %s", GetDebugName(component), GetDebugName(entity));
        return false;
    }

    if(!IsEntityValid(component)) {
        Log(0, LogSeverity_Error, "Cannot remove invalid Component");
        return false;
    }

    if(HasComponent(entity, component)) {
        for_entity(extension, extensionData, Extension) {
            if(extensionData->ExtensionComponent == component) {
                if(!extensionData->ExtensionDisabled) {
                    RemoveComponent(entity, extensionData->ExtensionExtenderComponent);
                }
            }
        }

		Type types[] = { TypeOf_Entity, TypeOf_Entity };
		const Variant values[] = { MakeVariant(Entity, component), MakeVariant(Entity, entity) };
		FireEventFast(EventOf_EntityComponentRemoved(), 2, values);

        if(HasComponent(entity, component)) {
            auto componentData = GetComponentType(GetEntityIndex(component));
            auto componentIndex = _GetComponentIndex(componentData, GetEntityIndex(entity));
            auto deletionEntityIndex = GetEntityIndex(entity);
            auto deletionComponentIndex = GetVector(componentData->EntityComponentIndices)[deletionEntityIndex];

            u32 count = 0;
            auto properties = GetProperties(component, &count);
            for(u32 i = 0; i < count; ++i) {
                auto property = properties[i];
                auto propertyData = GetPropertyData(property);

                auto kind = propertyData->PropertyKind;

                if(kind == PropertyKind_Child) {
                    auto offset = propertyData->PropertyOffset;
                    auto childEntity = *(Entity*)(componentData->DataBuffer[componentIndex] + sizeof(Entity)*2 + offset);

                    EmitChangedEvent(entity, property, propertyData, MakeVariant(Entity, childEntity), Variant_Default);
                    DestroyEntity(childEntity);
                } else if(kind == PropertyKind_Array) {
                    while(GetArrayPropertyCount(property, entity)) {
                        RemoveArrayPropertyElement(property, entity, 0);
                    }
                } else {
                    auto empty = Variant_Empty;
                    empty.type = propertyData->PropertyType;
                    SetPropertyValue(property, entity, empty); // Reset value to default before removal
                }
            }

            GetVector(componentData->EntityComponentIndices)[deletionEntityIndex] = InvalidIndex;

            memset(componentData->DataBuffer[deletionComponentIndex], 0, componentData->DataBuffer.GetElementSize());

            componentData->DataBuffer.Remove(deletionComponentIndex);
        }

        Verbose(Verbose_Component, "Component %s has been removed from Entity %s.", GetDebugName(component), GetDebugName(entity));

        return true;
    }
    return false;
}

API_EXPORT void SetComponentSize(Entity component, u16 value) {
    auto data = GetComponentData(component);
    Variant oldValue, newValue;
    if(data) {
        oldValue = MakeVariant(u16, data->ComponentSize);
        data->ComponentSize = value;
    }

    newValue = MakeVariant(u16, value);

    auto componentData = GetComponentType(GetEntityIndex(component));
    componentData->DataBuffer.SetElementSize(value + sizeof(Entity)*2);

    if(__isComponentInitialized) {
        EmitChangedEvent(component, PropertyOf_ComponentSize(), GetPropertyData(PropertyOf_ComponentSize()), oldValue, MakeVariant(u16, value));
    }
}

API_EXPORT u32 GetComponentIndex(Entity component, Entity entity) {
	auto type = GetComponentType(GetEntityIndex(component));
	return _GetComponentIndex(type, GetEntityIndex(entity));
}

API_EXPORT Entity GetComponentEntity(Entity component, u32 componentIndex) {
	auto type = GetComponentType(GetEntityIndex(component));
	return _GetComponentEntity(type, componentIndex);
}

__PropertyCoreGet(u16, ComponentSize, Component)
__PropertyCoreImpl(bool, ComponentExplicitSize, Component)
__ArrayPropertyCoreImpl(Property, Properties, Component)
__ArrayPropertyCoreImpl(Base, Bases, Component)

LocalFunction(OnEntityDestroyed, void, Entity entity) {
    for_entity(component, data, Component) {
        RemoveComponent(entity, component);
    }
}

static void RemoveExtensions(Entity component, Entity extensionComponent) {
    for_entity_abstract(entity, data, component) {
        RemoveComponent(entity, extensionComponent);
    }
}

static void AddExtensions(Entity component, Entity extensionComponent) {
    for_entity_abstract(entity, data, component) {
        AddComponent(entity, extensionComponent);
    }
}

LocalFunction(OnExtensionDisabledChanged,
        void,
        Entity extension,
        bool oldDisabled,
        bool newDisabled) {
    if(!newDisabled) {
        AddExtensions(GetExtensionComponent(extension), GetExtensionExtenderComponent(extension));
    }

    if(newDisabled) {
        RemoveExtensions(GetExtensionComponent(extension), GetExtensionExtenderComponent(extension));
    }
}

LocalFunction(OnExtensionComponentChanged,
        void,
        Entity extension,
        Entity oldValue,
        Entity newValue) {
    if(!GetExtensionDisabled(extension)) {
        RemoveExtensions(oldValue, GetExtensionExtenderComponent(extension));
        AddExtensions(newValue, GetExtensionExtenderComponent(extension));
    }
}

LocalFunction(OnExtensionExtenderComponentChanged,
        void,
        Entity extension,
        Entity oldValue,
        Entity newValue) {
    if(!GetExtensionDisabled(extension)) {
        RemoveExtensions(GetExtensionComponent(extension), oldValue);
        AddExtensions(GetExtensionComponent(extension), newValue);
    }
}

BeginUnit(Component)
    BeginComponent(Base)
        RegisterReferenceProperty(Component, BaseComponent)
    EndComponent()

    BeginComponent(Component)
        RegisterArrayProperty(Property, Properties)
        RegisterArrayProperty(Base, Bases)
        RegisterPropertyReadOnly(u16, ComponentSize)
        RegisterPropertyReadOnly(bool, ComponentExplicitSize)
    EndComponent()

    BeginComponent(Extension)
        RegisterReferenceProperty(Component, ExtensionComponent)
        RegisterReferenceProperty(Component, ExtensionExtenderComponent)
        RegisterProperty(bool, ExtensionDisabled)
    EndComponent()

    RegisterEvent(EntityComponentAdded)
    RegisterEvent(EntityComponentAdding)
    RegisterEvent(EntityComponentRemoved)

    __isComponentInitialized = true;

    RegisterSubscription(EventOf_EntityDestroyed(), OnEntityDestroyed, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionDisabled()), OnExtensionDisabledChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionComponent()), OnExtensionComponentChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionExtenderComponent()), OnExtensionExtenderComponentChanged, 0)
EndUnit()


void __InitializeComponent() {
    auto component = ComponentOf_Component();
    AddComponent(component, ComponentOf_Component());
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Properties(), InvalidIndex, 0, TypeOf_Entity, component, ComponentOf_Property(), PropertyKind_Array);
    __Property(PropertyOf_ComponentSize(), offsetof(Component, ComponentSize), sizeof(Component::ComponentSize), TypeOf_u16, component, 0, PropertyKind_Value);
}

void __PreInitialize() {
    memset(ComponentDataList, 0, sizeof(ComponentDataList));
}
