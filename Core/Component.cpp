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

struct ComponentInfo {
    Pool DataPool;
    eastl::vector<Entity> Dependees;
};

static eastl::vector<ComponentInfo> componentData;
static eastl::vector<EntityComponentIndexMap> entityComponentIndices;

API_EXPORT u32 GetComponentIndex(Entity component, Entity entity) {
	auto componentEntityIndex = GetEntityIndex(component);
	auto entityEntityIndex = GetEntityIndex(entity);

	if (entityEntityIndex >= entityComponentIndices.size()) {
		return InvalidIndex;
	}

	auto& map = entityComponentIndices[entityEntityIndex];
	auto it = map.find(component);
	if (it == map.end()) {
		return InvalidIndex;
	}

	return it->second;
}

API_EXPORT Pool& GetComponentPool(Entity component) {
	auto index = GetComponentIndex(ComponentOf_Component(), component);
	if (componentData.size() <= index) {
		componentData.resize(index + 1);
	}
	return componentData[index].DataPool;

}

API_EXPORT bool HasComponent (Entity entity, Entity component) {
	auto entityIndex = GetEntityIndex(entity);

	if (entityIndex >= entityComponentIndices.size()) {
		return false;
	}

	auto& entityComponentMap = entityComponentIndices[entityIndex];
	return entityComponentMap.find(component) != entityComponentMap.end();
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

		auto& pool = GetComponentPool(component);
        auto componentDataIndex = pool.Add();

        auto entityIndex = GetEntityIndex(entity);

        auto count = entityComponentIndices.size();
        if(count <= entityIndex) {
			entityComponentIndices.resize(entityIndex + 1);
        }

		entityComponentIndices[entityIndex][component] = componentDataIndex;

		memset(pool[componentDataIndex], 0, pool.GetElementSize());
        *(Entity*)pool[componentDataIndex] = entity;

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
                *(Entity*)(pool[componentDataIndex] + sizeof(Entity)*2 + offset) = child;

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

        auto& componentInfo = componentData[GetComponentIndex(ComponentOf_Component(), component)];

		for (auto& dependee : componentInfo.Dependees) {
			RemoveComponent(entity, dependee);
		}

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
            auto componentIndex = entityComponentIndices[entityIndex][component];

			auto deletionEntityIndex = entityIndex;
			auto deletionComponentIndex = componentIndex;

			auto& properties = GetProperties(component);
			for (u32 i = 0; i < properties.size(); ++i) {
				auto property = properties[i];
				auto propertyData = GetPropertyData(property);

				auto kind = propertyData->PropertyKind;

				if (kind == PropertyKind_Child) {
					auto offset = propertyData->PropertyOffset;
					auto childEntity = *(Entity*)(componentInfo.DataPool[componentIndex] + sizeof(Entity) * 2 + offset);

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
						auto stringValue = *(StringRef*)(componentInfo.DataPool[componentIndex] + sizeof(Entity) * 2 + propertyData->PropertyOffset);
						ReleaseStringRef(stringValue);
					}
					//SetPropertyValue(property, entity, empty); // Reset value to default before removal
				}
			}

            entityComponentIndices[entityIndex].erase(component);
            componentInfo.DataPool.Remove(componentIndex);
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

    auto& pool = GetComponentPool(component);
    pool.SetElementSize(value + sizeof(Entity)*2);

    if(__isComponentInitialized) {
        EmitChangedEvent(component, PropertyOf_ComponentSize(), GetPropertyData(PropertyOf_ComponentSize()), oldValue, MakeVariant(u16, value));
    }
}

API_EXPORT Entity GetComponentEntity(Entity component, u32 componentIndex) {
    return *(Entity*)GetComponentPool(component)[componentIndex];
}

API_EXPORT char* GetComponentData(Entity component, u32 componentIndex) {
	if (componentIndex == InvalidIndex) return 0;

    return GetComponentPool(component)[componentIndex] + sizeof(Entity) * 2;
}

API_EXPORT const EntityComponentIndexMap& GetEntityComponents(Entity entity) {
	return entityComponentIndices[GetEntityIndex(entity)];
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
		auto componentDataIndex = GetComponentIndex(ComponentOf_Component(), oldComponent);
        auto& componentInfo = componentData[componentDataIndex];

		eastl::remove(componentInfo.Dependees.begin(), componentInfo.Dependees.end(), GetOwner(base));
	}

	if (newComponent) {
		AddComponent(newComponent, ComponentOf_Component());
		GetComponentPool(newComponent);
		auto componentDataIndex = GetComponentIndex(ComponentOf_Component(), newComponent);
        auto& componentInfo = componentData[componentDataIndex];
        componentInfo.Dependees.push_back(GetOwner(base));
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

#define BootstrapComponent(COMPONENT) \
	{\
		auto componentIndex = componentData.size();\
		auto entity = ComponentOf_ ## COMPONENT ();\
		componentData.push_back(ComponentInfo());\
		\
		componentData[componentIndex].DataPool.SetElementSize(sizeof(Entity) * 2 + sizeof(COMPONENT));\
		entityComponentIndices[GetEntityIndex(entity)][ComponentOf_Component()] = componentIndex;\
		\
		componentIndex = componentData[0].DataPool.Add();\
	    Component *data = (Component*)componentData[0].DataPool[componentIndex] + sizeof(Entity) * 2;\
		((Entity*)data)[-2] = entity;\
		data->ComponentExplicitSize = true;\
		data->ComponentSize = sizeof(COMPONENT);\
		data->ComponentDataIndex = componentIndex;\
	}

void __InitializeComponent() {
	auto component = ComponentOf_Component();

	entityComponentIndices.resize(1024);

	BootstrapComponent(Component)
	BootstrapComponent(Ownership)
	BootstrapComponent(Identification)
	BootstrapComponent(Property)

    AddComponent(component, ComponentOf_Component());
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Properties(), InvalidIndex, 0, TypeOf_Entity, component, ComponentOf_Property(), PropertyKind_Array, "Properties");
    __Property(PropertyOf_ComponentSize(), offsetof(Component, ComponentSize), sizeof(Component::ComponentSize), TypeOf_u16, component, 0, PropertyKind_Value, "ComponentSize");
	SetUuid(component, "Component.Component");
}

void __PreInitialize() {
}
