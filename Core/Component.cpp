//
// Created by Kim on 03/06/2018.
//

#include <Core/NativeUtils.h>
#include "Identification.h"
#include "Component.h"

#include <EASTL/map.h>
#include <EASTL/fixed_map.h>

static bool __isComponentInitialized = false;

#define Verbose_Component "component"

static const u32 expectedMaxComponentCount = 256;

static eastl::fixed_map<Entity, ComponentTypeData, expectedMaxComponentCount> ComponentDataMap;
static eastl::fixed_map<Entity, eastl::vector<Entity>, expectedMaxComponentCount> componentDependees;

static eastl::map<Entity, EntityComponentList> entityComponents;

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

API_EXPORT ComponentTypeData* GetComponentType(Entity componentEntity) {
	return &ComponentDataMap[componentEntity];
}

API_EXPORT bool HasComponent (Entity entity, Entity component) {
    auto componentData = GetComponentType(component);
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

        auto componentData = GetComponentType(component);

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
		
		entityComponents[entity].push_back(component);

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
		auto entityIndex = GetEntityIndex(entity);

		for (auto& dependees : componentDependees[component]) {
			RemoveComponent(entity, dependees);
		}

		eastl::remove(entityComponents[entity].begin(), entityComponents[entity].end(), component);

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

		if (HasComponent(entity, component)) {
			auto componentData = GetComponentType(component);
			auto componentIndex = _GetComponentIndex(componentData, entityIndex);
			auto deletionEntityIndex = entityIndex;
			auto deletionComponentIndex = GetVector(componentData->EntityComponentIndices)[deletionEntityIndex];

			auto& properties = GetProperties(component);
			for (u32 i = 0; i < properties.size(); ++i) {
				auto property = properties[i];
				auto propertyData = GetPropertyData(property);

				auto kind = propertyData->PropertyKind;

				if (kind == PropertyKind_Child) {
					auto offset = propertyData->PropertyOffset;
					auto childEntity = *(Entity*)(componentData->DataBuffer[componentIndex] + sizeof(Entity) * 2 + offset);

					EmitChangedEvent(entity, property, propertyData, MakeVariant(Entity, childEntity), Variant_Default);
					DestroyEntity(childEntity);
				}
				else if (kind == PropertyKind_Array) {
					while (GetArrayPropertyCount(property, entity)) {
						RemoveArrayPropertyElement(property, entity, 0);
					}
				}
				else {
					auto empty = Variant_Empty;
					empty.type = propertyData->PropertyType;

					if (propertyData->PropertyType == TypeOf_StringRef) {
						auto stringValue = *(StringRef*)(componentData->DataBuffer[componentIndex] + sizeof(Entity) * 2 + propertyData->PropertyOffset);
						ReleaseStringRef(stringValue);
					}
					//SetPropertyValue(property, entity, empty); // Reset value to default before removal
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

    auto componentData = GetComponentType(component);
    componentData->DataBuffer.SetElementSize(value + sizeof(Entity)*2);

    if(__isComponentInitialized) {
        EmitChangedEvent(component, PropertyOf_ComponentSize(), GetPropertyData(PropertyOf_ComponentSize()), oldValue, MakeVariant(u16, value));
    }
}

API_EXPORT u32 GetComponentIndex(Entity component, Entity entity) {
	auto type = GetComponentType(component);
	return _GetComponentIndex(type, GetEntityIndex(entity));
}

API_EXPORT Entity GetComponentEntity(Entity component, u32 componentIndex) {
	auto type = GetComponentType(component);
	return _GetComponentEntity(type, componentIndex);
}

API_EXPORT const EntityComponentList& GetEntityComponents(Entity entity) {
	return entityComponents[entity];
}

__PropertyCoreGet(u16, ComponentSize, Component)
__PropertyCoreImpl(bool, ComponentExplicitSize, Component)
__ArrayPropertyCoreImpl(Property, Properties, Component)
__ArrayPropertyCoreImpl(Base, Bases, Component)

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

LocalFunction(OnBaseComponentChanged, void, Entity base, Entity oldComponent, Entity newComponent) {
	if (oldComponent) {
		auto& arr = componentDependees[oldComponent];

		eastl::remove(arr.begin(), arr.end(), GetOwner(base));
	}

	if (newComponent) {
		componentDependees[newComponent].push_back(GetOwner(base));
	}
}

LocalFunction(OnBaseRemoved, void, Entity component, Entity base) {
	OnBaseComponentChanged(base, GetBaseComponent(base), 0);
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

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionDisabled()), OnExtensionDisabledChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionComponent()), OnExtensionComponentChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ExtensionExtenderComponent()), OnExtensionExtenderComponentChanged, 0)

	RegisterSubscription(GetPropertyChangedEvent(PropertyOf_BaseComponent()), OnBaseComponentChanged, 0)
	RegisterSubscription(EventOf_EntityComponentRemoved(), OnBaseRemoved, ComponentOf_Base())
EndUnit()


void __InitializeComponent() {
    auto component = ComponentOf_Component();
    AddComponent(component, ComponentOf_Component());
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Properties(), InvalidIndex, 0, TypeOf_Entity, component, ComponentOf_Property(), PropertyKind_Array, "Properties");
    __Property(PropertyOf_ComponentSize(), offsetof(Component, ComponentSize), sizeof(Component::ComponentSize), TypeOf_u16, component, 0, PropertyKind_Value, "ComponentSize");
	SetUuid(component, "Component.Component");
}

void __PreInitialize() {
}
