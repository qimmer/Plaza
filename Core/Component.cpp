//
// Created by Kim on 03/06/2018.
//

#include <Core/NativeUtils.h>
#include "Identification.h"
#include "Component.h"

#include <EASTL/map.h>
#include <EASTL/fixed_map.h>

#define Verbose_Component "component"

struct ComponentInfo {
    Pool DataPool;
    eastl::vector<Entity> ComponentIndexToEntityTable;
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
		componentInfo.ComponentIndexToEntityTable.resize(entityIndex + 1, 0);
    }

	componentInfo.EntityIndexToComponentIndexTable[entityIndex] = componentDataIndex;
	componentInfo.ComponentIndexToEntityTable[componentDataIndex] = entity;

	// Nullify new component data
	auto data = pool[componentDataIndex];
	memset(data, 0, pool.GetElementSize());

    auto& componentComponentData = *(Component*)componentData[0].DataPool[componentInfoIndex];

	for(auto base : componentComponentData.Bases) {
		AddComponent(entity, GetBase(base).BaseComponent);
	}

    NotifyChange(entity, component, data, data);

	Extension extensionData;
	for_entity_data(extension, ComponentOf_Extension(), &extensionData) {
        if(extensionData.ExtensionComponent == component) {
            if(!extensionData.ExtensionDisabled) {
                AddComponent(entity, extensionData.ExtensionExtenderComponent);
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

    Extension extensionData;
    for_entity_data(extension, ComponentOf_Extension(), &extensionData) {
		if (extensionData.ExtensionComponent == component) {
			if (!extensionData.ExtensionDisabled) {
				RemoveComponent(entity, extensionData.ExtensionExtenderComponent);
			}
		}
	}

    auto& componentComponentData = *(Component*)componentData[0].DataPool[componentInfoIndex];

    auto componentIndex = componentInfo.EntityIndexToComponentIndexTable[entityIndex];
	if (componentIndex != InvalidIndex) {
	    auto data = componentInfo.DataPool[componentIndex];

	    auto nullData = (char*)alloca(componentInfo.DataPool.GetElementSize());
	    memset(nullData, 0, componentInfo.DataPool.GetElementSize());

        NotifyChange(entity, component, data, nullData);

        for_children(property, PropertyOf_Properties(), component) {
            auto propertyData = GetProperty(property);
            if(propertyData.PropertyType == TypeOf_ChildArray) {
                SetArrayPropertyCount(property, entity, 0);
            }

            // Handle ref counting for strings
            else if(propertyData.PropertyType == TypeOf_StringRef) {
                auto& oldString = *(StringRef*)(data + propertyData.PropertyOffset);

                if(oldString) {
                    ReleaseStringRef(oldString);
                }
            }

            else if(propertyData.PropertyType == TypeOf_Entity) {
                // If a reference was changed and the referred entity is owned by us, destroy it to prevent dangling entity!
                Entity& oldRef = *(Entity*)(data + propertyData.PropertyOffset);
                if(IsEntityValid(oldRef)) {
                    auto refOwnership = GetOwnership(oldRef);
                    if(refOwnership.Owner == entity && refOwnership.OwnerProperty == property) {
                        DestroyEntity(oldRef);
                    }
                }
            }
        }

		componentInfo.EntityIndexToComponentIndexTable[entityIndex] = InvalidIndex;
		componentInfo.ComponentIndexToEntityTable[componentIndex] = 0;
		componentInfo.DataPool.Remove(componentIndex);
	}

	Verbose(Verbose_Component, "Component %s has been removed from Entity %s.", GetDebugName(component), GetDebugName(entity));

	return true;
}

API_EXPORT Entity GetComponentEntity(u32 componentInfoIndex, u32 componentIndex) {
    return *(Entity*)GetComponentPool(componentInfoIndex)[componentIndex];
}

API_EXPORT const void* GetComponentInstanceData(u32 componentInfoIndex, u32 componentIndex) {
	if (componentIndex == InvalidIndex) return 0;

    return componentData[componentInfoIndex].DataPool[componentIndex];
}

API_EXPORT void SetComponentInstanceData(u32 componentInfoIndex, u32 componentIndex, const void* newData) {
	if (componentIndex == InvalidIndex) return;

	auto component = componentData[0].ComponentIndexToEntityTable[componentInfoIndex];
	auto entity = componentData[componentInfoIndex].ComponentIndexToEntityTable[componentIndex];
	auto data = componentData[componentInfoIndex].DataPool[componentIndex];
	auto size = componentData[componentInfoIndex].DataPool.GetElementSize();

	auto oldData = (char*)alloca(size);
	memcpy(oldData, data, size);
	memcpy(data, newData, size);

	for_children(property, PropertyOf_Properties(), component) {
	    auto propertyData = GetProperty(property);
	    if(propertyData.PropertyType == TypeOf_ChildArray) {
	        memcpy(data + propertyData.PropertyOffset, oldData + propertyData.PropertyOffset, sizeof(ChildArray));
	    }

        // Handle ref counting for strings
        else if(propertyData.PropertyType == TypeOf_StringRef) {
            auto oldString = *(StringRef*)(oldData + propertyData.PropertyOffset);
            auto newString = Intern(*(StringRef*)(data + propertyData.PropertyOffset));

            if(newString) {
                AddStringRef(newString);
            }

            if(oldString) {
                ReleaseStringRef(oldString);
            }
        }

        else if(propertyData.PropertyType == TypeOf_Entity) {
            // If a reference was changed and the referred entity is owned by us, release ownership!
            Entity oldRef = *(Entity*)(oldData + propertyData.PropertyOffset);
            if(IsEntityValid(oldRef)) {
                auto refOwnership = GetOwnership(oldRef);
                if(refOwnership.Owner == entity && refOwnership.OwnerProperty == property) {
                    SetOwnership(oldRef, {0, 0});
                }
            }

            // If new reference has no owner, set ownership to our changing entity!
            Entity newRef = *(Entity*)(data + propertyData.PropertyOffset);
            if(IsEntityValid(newRef)) {
                auto refOwnership = GetOwnership(newRef);
                if(!refOwnership.Owner || !refOwnership.OwnerProperty) {
                    refOwnership.Owner = entity;
                    refOwnership.OwnerProperty = property;
                    SetOwnership(newRef, refOwnership);
                }
            }
        }
	}

	NotifyChange(entity, component, oldData, newData);
}


API_EXPORT void SetComponentData(Entity entity, Entity component, const void *data) {
	auto componentEntityIndex = GetEntityIndex(component);
	auto componentInfoIndex = componentData[0].EntityIndexToComponentIndexTable[componentEntityIndex];

	SetComponentInstanceData(componentInfoIndex, GetComponentIndexByIndex(componentInfoIndex, entity), data);
}

static void RemoveComponentExtensions(Entity component, Entity extensionComponent) {
    for_entity(entity, component) {
        RemoveComponent(entity, extensionComponent);
    }
}

static void AddComponentExtensions(Entity component, Entity extensionComponent) {
    for_entity(entity, component) {
        AddComponent(entity, extensionComponent);
    }
}

static void OnComponentChanged(Entity component, const Component& value, const Component& oldValue) {
	if(oldValue.ComponentSize != value.ComponentSize && value.ComponentExplicitSize) {
		auto componentTypeIndex = GetComponentIndexByIndex(0, component);
		auto& pool = GetComponentPool(componentTypeIndex);
		pool.SetElementSize(value.ComponentSize); // Entity, some padding, then new data and last, old data before system update
	}
}

static void OnExtensionChanged(Entity extension, const Extension& value, const Extension& oldValue) {
	if(!oldValue.ExtensionDisabled) {
		RemoveComponentExtensions(oldValue.ExtensionComponent, oldValue.ExtensionExtenderComponent);
	}

	if(!value.ExtensionDisabled) {
		AddComponentExtensions(value.ExtensionComponent, oldValue.ExtensionExtenderComponent);
	}
}

static void OnBaseChanged(Entity base, const Base& value, const Base& oldValue) {
	if (oldValue.BaseComponent) {
		auto componentDataIndex = GetComponentIndexByIndex(0, oldValue.BaseComponent);
        auto& componentInfo = componentData[componentDataIndex];

		eastl::remove(componentInfo.Dependees.begin(), componentInfo.Dependees.end(), GetOwnership(base).Owner);
	}

	if (value.BaseComponent) {
		AddComponent(value.BaseComponent, ComponentOf_Component());
		auto componentDataIndex = GetComponentIndexByIndex(0, value.BaseComponent);

		if (componentDataIndex >= componentData.size() && componentDataIndex != InvalidIndex) {
			componentData.resize(componentDataIndex + 1);
		}

        auto& componentInfo = componentData[componentDataIndex];
        componentInfo.Dependees.push_back(GetOwnership(base).Owner);
	}
}

BeginUnit(Component)
    RegisterSystem(OnComponentChanged, ComponentOf_Component())
    RegisterSystem(OnExtensionChanged, ComponentOf_Extension())
    RegisterSystem(OnBaseChanged, ComponentOf_Base())

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
    SetOwnership(component, {ModuleOf_Core(), PropertyOf_Components()});
    SetIdentification(component, {"Component.Component"});
}

void __PreInitialize() {
}
