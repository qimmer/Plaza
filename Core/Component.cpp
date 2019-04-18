//
// Created by Kim on 03/06/2018.
//

#include <Core/NativeUtils.h>
#include "Identification.h"
#include "Component.h"
#include "Instance.h"

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

static char nullData[SHRT_MAX];

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

    if(componentComponentData.ComponentDefaultData) {
        SetComponentInstanceData(componentInfoIndex, componentDataIndex, componentComponentData.ComponentDefaultData);
    } else {
        NotifyChange(entity, component, data, data);
    }

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

        if(componentComponentData.ComponentDefaultData) {
            SetComponentInstanceData(componentInfoIndex, componentIndex, componentComponentData.ComponentDefaultData);
        } else {
            SetComponentInstanceData(componentInfoIndex, componentIndex, nullData);
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
    const Component& componentComponentData = *(const Component*)componentData[0].DataPool[componentInfoIndex];

	if (componentIndex == InvalidIndex) return componentComponentData.ComponentDefaultData ? componentComponentData.ComponentDefaultData : nullData;

    return componentData[componentInfoIndex].DataPool[componentIndex];
}

API_EXPORT void SetComponentInstanceData(u32 componentInfoIndex, u32 componentIndex, const void* newData) {
	if (componentIndex == InvalidIndex) return;

	auto component = componentData[0].ComponentIndexToEntityTable[componentInfoIndex];
	auto entity = componentData[componentInfoIndex].ComponentIndexToEntityTable[componentIndex];
	auto data = componentData[componentInfoIndex].DataPool[componentIndex];
	auto size = componentData[componentInfoIndex].DataPool.GetElementSize();
	const Component& componentComponentData = *(const Component*)componentData[0].DataPool[componentInfoIndex];

	auto oldData = (char*)alloca(size);
	memcpy(oldData, data, size);
	memcpy(data, newData, size);

	for(auto property : componentComponentData.Properties) {
	    auto propertyData = GetProperty(property);
	    if(propertyData.PropertyType == TypeOf_ChildArray) {
            auto& oldArray = *(ChildArray*)(oldData + propertyData.PropertyOffset);
            auto& newArray = *(ChildArray*)(data + propertyData.PropertyOffset);

            if(newArray.IsDetached()) {
                newArray.Attach(); // Make sure we retain array memory between frames on new stored array
                oldArray.Detach();
            }

            if(oldArray.GetSize() != newArray.GetSize() || memcmp(oldArray.begin(), newArray.begin(), sizeof(Entity) * oldArray.GetSize()) != 0) {
                // Remove ownership from removed elements
                for(auto child : oldArray) {
                    auto ownershipData = GetOwnership(child);
                    auto index = newArray.GetIndex(child);
                    if(index == InvalidIndex && ownershipData.Owner == entity && ownershipData.OwnerProperty == property) {
                        ownershipData.Owner = 0;
                        ownershipData.OwnerProperty = 0;
                        SetOwnership(child, ownershipData);
                    }
                }

                // Add ownership to added orphans
                for(auto child : newArray) {
                    auto ownershipData = GetOwnership(child);
                    auto index = oldArray.GetIndex(child);
                    if(index == InvalidIndex && !ownershipData.Owner && !ownershipData.OwnerProperty) {
                        ownershipData.Owner = entity;
                        ownershipData.OwnerProperty = property;
                        SetOwnership(child, ownershipData);
                    }
                }
            }
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
            if(propertyData.PropertyPrefab && !*(Entity*)(data + propertyData.PropertyOffset)) {
                // Prevent changing child properties from outside
                *(Entity*)(data + propertyData.PropertyOffset) = *(Entity*)(oldData + propertyData.PropertyOffset);

                // If we are still null, component has just been added and we should initialize child!
                if(!*(Entity*)(data + propertyData.PropertyOffset)) {
                    auto child = CreateEntity();
                    *(Entity*)(data + propertyData.PropertyOffset) = child;

                    SetOwnership(child, {entity, property});
                    SetInstance(child, {propertyData.PropertyPrefab});
                }
            }

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

	NotifyChange(entity, component, oldData, data);
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

static void OnComponentChanged(Entity component, const Component& oldValue, const Component& newValue) {
	if(oldValue.ComponentSize != newValue.ComponentSize && newValue.ComponentExplicitSize) {
		auto componentTypeIndex = GetComponentIndexByIndex(0, component);
		auto& pool = GetComponentPool(componentTypeIndex);
		pool.SetElementSize(newValue.ComponentSize); // Entity, some padding, then new data and last, old data before system update
	}
}

static void OnExtensionChanged(Entity extension, const Extension& oldValue, const Extension& newValue) {
	if(!oldValue.ExtensionDisabled) {
		RemoveComponentExtensions(oldValue.ExtensionComponent, oldValue.ExtensionExtenderComponent);
	}

	if(!newValue.ExtensionDisabled) {
		AddComponentExtensions(newValue.ExtensionComponent, oldValue.ExtensionExtenderComponent);
	}
}

static void OnBaseChanged(Entity base, const Base& oldValue, const Base& newValue) {
	if (oldValue.BaseComponent) {
		auto componentDataIndex = GetComponentIndexByIndex(0, oldValue.BaseComponent);
        auto& componentInfo = componentData[componentDataIndex];

		eastl::remove(componentInfo.Dependees.begin(), componentInfo.Dependees.end(), GetOwnership(base).Owner);
	}

	if (newValue.BaseComponent) {
		AddComponent(newValue.BaseComponent, ComponentOf_Component());
		auto componentDataIndex = GetComponentIndexByIndex(0, newValue.BaseComponent);

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
        RegisterProperty(NativePtr, ComponentDefaultData)
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
    memset(nullData, 0, sizeof(nullData));

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
