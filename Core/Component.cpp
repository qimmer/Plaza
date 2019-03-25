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
	eastl::vector<u32> EntityIndexToComponentIndexTable;
	eastl::vector<Entity> Dependees;
};

static eastl::vector<ComponentInfo> componentData;

API_EXPORT u32 GetComponentInfoIndex(Entity component) {
	auto componentEntityIndex = GetEntityIndex(component);
	return componentData[0].EntityIndexToComponentIndexTable[componentEntityIndex];
}

API_EXPORT u32 GetComponentIndex(Entity component, Entity entity) {
	auto entityEntityIndex = GetEntityIndex(entity);
	auto componentEntityIndex = GetEntityIndex(component);
	auto componentInfoIndex = componentData[0].EntityIndexToComponentIndexTable[componentEntityIndex];

	if (componentInfoIndex == InvalidIndex) {
		return InvalidIndex;
	}

	auto& table = componentData[componentInfoIndex].EntityIndexToComponentIndexTable;
	if (entityEntityIndex >= table.size()) {
		return InvalidIndex;
	}
	
	return table[entityEntityIndex];
}

API_EXPORT u32 GetComponentIndexByIndex(u32 componentInfoIndex, Entity entity) {
	auto entityEntityIndex = GetEntityIndex(entity);
	auto& table = componentData[componentInfoIndex].EntityIndexToComponentIndexTable;
	if (entityEntityIndex >= table.size()) {
		return InvalidIndex;
	}

	return table[entityEntityIndex];
}

API_EXPORT Pool& GetComponentPool(u32 componentInfoIndex) {
	if (componentInfoIndex >= componentData.size() && componentInfoIndex != InvalidIndex) {
		componentData.resize(componentInfoIndex + 1);
	}

	return componentData[componentInfoIndex].DataPool;

}

API_EXPORT bool HasComponent (Entity entity, Entity component) {
	auto entityIndex = GetEntityIndex(entity);
	auto componentEntityIndex = GetEntityIndex(component);

	auto componentInfoIndex = componentData[0].EntityIndexToComponentIndexTable[componentEntityIndex];

	auto& table = componentData[componentInfoIndex].EntityIndexToComponentIndexTable;
	if (entityIndex >= table.size()) {
		return false;
	}

	return table[entityIndex] != InvalidIndex;
}

API_EXPORT bool AddComponent (Entity entity, Entity component) {
	auto entityIndex = GetEntityIndex(entity);
	auto componentEntityIndex = GetEntityIndex(component);
	auto componentInfoIndex = componentData[0].EntityIndexToComponentIndexTable[componentEntityIndex];
	auto& componentInfo = componentData[componentInfoIndex];

	auto indexTableSize = componentInfo.EntityIndexToComponentIndexTable.size();
	bool hasComponent = indexTableSize > entityIndex && componentInfo.EntityIndexToComponentIndexTable[entityIndex] != InvalidIndex;
	if (hasComponent) {
		return false;
	}
	
    if(!IsEntityValid(entity)) {
        Log(0, LogSeverity_Error, "Invalid Entity when adding component %s: %s", GetDebugName(component), GetDebugName(entity));
        return false;
    }

	if (!IsEntityValid(component)) {
		Log(0, LogSeverity_Error, "Invalid Component when removing component %s: %s", GetDebugName(component), GetDebugName(entity));
		return false;
	}

	auto& pool = componentInfo.DataPool;
    auto componentDataIndex = pool.Add();

    if(indexTableSize <= entityIndex) {
		componentInfo.EntityIndexToComponentIndexTable.resize(entityIndex + 1, InvalidIndex);
    }

	componentInfo.EntityIndexToComponentIndexTable[entityIndex] = componentDataIndex;

	auto data = pool[componentDataIndex];
	memset(data, 0, pool.GetElementSize());
    *(Entity*)data = entity;

    if(__isComponentInitialized) {
        Variant arguments[] = {MakeVariant(Entity, component), MakeVariant(Entity, entity)};

        auto parentUuid = GetUuid(entity);

        // Initialize children
        for_children(property, Properties, component) {
            auto propertyData = GetPropertyData(property);
            if(propertyData->PropertyKind != PropertyKind_Child) continue;

            auto child = CreateEntity();
            auto propertyName = strrchr(GetUuid(property), '.') + 1;

            SetUuid(child, StringFormatV("%s.%s", parentUuid, propertyName));

            auto offset = propertyData->PropertyOffset;
            *(Entity*)(data + sizeof(Entity)*2 + offset) = child;

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

API_EXPORT bool RemoveComponent (Entity entity, Entity component) {
	auto entityIndex = GetEntityIndex(entity);
	auto componentEntityIndex = GetEntityIndex(component);
	auto componentInfoIndex = componentData[0].EntityIndexToComponentIndexTable[componentEntityIndex];
	auto& componentInfo = componentData[componentInfoIndex];

	auto indexTableSize = componentInfo.EntityIndexToComponentIndexTable.size();
	bool hasComponent = indexTableSize > entityIndex && componentInfo.EntityIndexToComponentIndexTable[entityIndex] != InvalidIndex;
	if (!hasComponent) {
		return false;
	}
	
    if(!IsEntityValid(entity)) {
        Log(0, LogSeverity_Error, "Invalid Entity when removing component %s: %s", GetDebugName(component), GetDebugName(entity));
        return false;
    }

	if (!IsEntityValid(component)) {
		Log(0, LogSeverity_Error, "Invalid Component when removing component %s: %s", GetDebugName(component), GetDebugName(entity));
		return false;
	}

	for (auto& dependee : componentInfo.Dependees) {
		RemoveComponent(entity, dependee);
	}

	for_entity(extension, extensionData, Extension) {
		if (extensionData->ExtensionComponent == component) {
			if (!extensionData->ExtensionDisabled) {
				RemoveComponent(entity, extensionData->ExtensionExtenderComponent);
			}
		}
	}

	Type types[] = { TypeOf_Entity, TypeOf_Entity };
	const Variant values[] = { MakeVariant(Entity, component), MakeVariant(Entity, entity) };
	FireEventFast(EventOf_EntityComponentRemoved(), 2, values);

	auto componentIndex = componentInfo.EntityIndexToComponentIndexTable[entityIndex];
	if (componentIndex != InvalidIndex) {
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

				SetPropertyValue(property, entity, empty); // Reset value to default before removal
			}
		}

		componentInfo.EntityIndexToComponentIndexTable[entityIndex] = InvalidIndex;
		componentInfo.DataPool.Remove(componentIndex);
	}

	Verbose(Verbose_Component, "Component %s has been removed from Entity %s.", GetDebugName(component), GetDebugName(entity));

	return true;
}

API_EXPORT void SetComponentSize(Entity component, u16 value) {
    auto data = GetComponentData(component);
    Variant oldValue, newValue;
    if(data) {
        oldValue = MakeVariant(u16, data->ComponentSize);
        data->ComponentSize = value;
    }

    newValue = MakeVariant(u16, value);

	auto componentTypeIndex = GetComponentIndexByIndex(0, component);
    auto& pool = GetComponentPool(componentTypeIndex);
    pool.SetElementSize(value + sizeof(Entity)*2);

    if(__isComponentInitialized) {
        EmitChangedEvent(component, PropertyOf_ComponentSize(), GetPropertyData(PropertyOf_ComponentSize()), oldValue, MakeVariant(u16, value));
    }
}

API_EXPORT Entity GetComponentEntity(u32 componentInfoIndex, u32 componentIndex) {
    return *(Entity*)GetComponentPool(componentInfoIndex)[componentIndex];
}

API_EXPORT char* GetComponentInstanceData(u32 componentInfoIndex, u32 componentIndex) {
	if (componentIndex == InvalidIndex) return 0;

    return componentData[componentInfoIndex].DataPool[componentIndex] + sizeof(Entity) * 2;
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
	auto extenderComponent = GetExtensionExtenderComponent(extension);
	auto extensionComponent = GetExtensionComponent(extension);

	if (!extenderComponent || !extensionComponent) return;

    if(!newDisabled) {
        AddExtensions(extensionComponent, extenderComponent);
    }

    if(newDisabled) {
        RemoveExtensions(extensionComponent, extenderComponent);
    }
}

LocalFunction(OnExtensionComponentChanged,
        void,
        Entity extension,
        Entity oldValue,
        Entity newValue) {
	auto extenderComponent = GetExtensionExtenderComponent(extension);

    if(!GetExtensionDisabled(extension) && extenderComponent) {
        RemoveExtensions(oldValue, extenderComponent);
        AddExtensions(newValue, extenderComponent);
    }
}

LocalFunction(OnExtensionExtenderComponentChanged,
        void,
        Entity extension,
        Entity oldValue,
        Entity newValue) {
	auto extensionComponent = GetExtensionComponent(extension);

    if(!GetExtensionDisabled(extension) && extensionComponent) {
        RemoveExtensions(extensionComponent, oldValue);
        AddExtensions(extensionComponent, newValue);
    }
}

LocalFunction(OnBaseComponentChanged, void, Entity base, Entity oldComponent, Entity newComponent) {
	if (oldComponent) {
		auto componentDataIndex = GetComponentIndexByIndex(0, oldComponent);
        auto& componentInfo = componentData[componentDataIndex];

		eastl::remove(componentInfo.Dependees.begin(), componentInfo.Dependees.end(), GetOwner(base));
	}

	if (newComponent) {
		AddComponent(newComponent, ComponentOf_Component());
		auto componentDataIndex = GetComponentIndexByIndex(0, newComponent);

		if (componentDataIndex >= componentData.size() && componentDataIndex != InvalidIndex) {
			componentData.resize(componentDataIndex + 1);
		}

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
		auto componentIndex = (u32)componentData.size();\
		auto entity = ComponentOf_ ## COMPONENT ();\
		componentData.push_back(ComponentInfo());\
		\
		componentData[componentIndex].DataPool.SetElementSize(sizeof(Entity) * 2 + sizeof(COMPONENT));\
		auto entityIndex = GetEntityIndex(entity);\
		if(entityIndex >= componentData[0].EntityIndexToComponentIndexTable.size()) {\
			componentData[0].EntityIndexToComponentIndexTable.resize(entityIndex + 1);\
		}\
		componentData[0].EntityIndexToComponentIndexTable[entityIndex] = componentIndex;\
		\
		componentIndex = componentData[0].DataPool.Add();\
	    Component *data = (Component*)(componentData[0].DataPool[componentIndex] + sizeof(Entity) * 2);\
		((Entity*)data)[-2] = entity;\
		data->ComponentExplicitSize = true;\
		data->ComponentSize = sizeof(COMPONENT);\
		data->ComponentDataIndex = componentIndex;\
	}

void __InitializeComponent() {
	auto component = ComponentOf_Component();
	
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
