//
// Created by Kim on 03/06/2018.
//

#include <Core/Component.h>
#include <Core/Function.h>
#include <Core/Entity.h>
#include <Core/Event.h>
#include <Core/Pool.h>
#include "Identification.h"

static bool __isComponentInitialized = false;

struct ComponentTypeData {
    Pool DataBuffer;
    Vector<u32> EntityComponentIndices;
};

API_EXPORT Vector<ComponentTypeData> ComponentDataList;
API_EXPORT Vector<u16> ComponentDataIndices;

struct Extension {
    Entity ExtensionComponent, ExtensionExtenderComponent;
    bool ExtensionDisabled;
};

struct Base {
    Entity BaseComponent;
};

struct Component {
    Entity ComponentTemplate;
    Vector(Properties, Entity, 32)
    Vector(Bases, Entity, 8)
    u16 ComponentDataIndex;
    u16 ComponentSize;
    bool ComponentExplicitSize;
};

static ComponentTypeData *GetComponentType(Entity component) {
    auto entityIndex = GetEntityIndex(component);
    if(entityIndex >= ComponentDataIndices.size()) {
        ComponentDataIndices.resize(UpperPowerOf2(Max(entityIndex + 1, 8)), UINT16_MAX);
    }

    auto componentIndex = ComponentDataIndices[entityIndex];
    if(componentIndex == UINT16_MAX) {
        ComponentDataList.resize(ComponentDataList.size() + 1);
        ComponentDataIndices[entityIndex] = componentIndex = ComponentDataList.size() - 1;
    }

    return &ComponentDataList[componentIndex];
}

API_EXPORT u32 GetComponentMax (Entity component) {
    return GetComponentType(component)->DataBuffer.End();
}

API_EXPORT Entity GetComponentEntity(Entity component, u32 index) {
    auto componentData = GetComponentType(component);

    if(!componentData->DataBuffer.IsValid(index)) {
        return 0;
    }

    return *(Entity*)componentData->DataBuffer[index];
}

API_EXPORT u32 GetComponentIndex(Entity component, Entity entity) {
    if(!IsEntityValid(component)) {
        Log(0, LogSeverity_Error, "Cannot get index of invalid component");
        return InvalidIndex;
    }

    if(!IsEntityValid(entity)) {
        return InvalidIndex;
    }

    auto componentData = GetComponentType(component);
    auto entityIndex = GetEntityIndex(entity);

    if(componentData->EntityComponentIndices.size() <= entityIndex) {
        componentData->EntityComponentIndices.resize(entityIndex + 1, InvalidIndex);
    }

    return componentData->EntityComponentIndices[entityIndex];
}

API_EXPORT bool HasComponent (Entity entity, Entity component) {
    return GetComponentIndex(component, entity) != InvalidIndex;
}

API_EXPORT char * GetComponentBytes(Entity component, u32 index) {
    if(index == InvalidIndex || !IsEntityValid(component)) return NULL;

    auto componentData = GetComponentType(component);
    return componentData->DataBuffer[index] + sizeof(Entity);
}

API_EXPORT bool AddComponent (Entity entity, Entity component) {
    if(!IsEntityValid(entity)) {
        Log(0, LogSeverity_Error, "Invalid Entity when adding component %s: %s", GetDebugName(component), GetDebugName(entity));
        return false;
    }

    if(!IsEntityValid(component)) {
        Log(0, LogSeverity_Error, "Cannot add invalid Component");
        return false;
    }

    if(!HasComponent (entity, component)) {
        auto componentData = GetComponentType(component);

        auto componentIndex = componentData->DataBuffer.Add();
        auto entityIndex = GetEntityIndex(entity);

        if(componentData->EntityComponentIndices.size() <= entityIndex) {
            componentData->EntityComponentIndices.resize(entityIndex + 1, InvalidIndex);
        }

        componentData->EntityComponentIndices[entityIndex] = componentIndex;
        *(Entity*)componentData->DataBuffer[componentIndex] = entity;

        auto dataSize = GetComponentSize(component);
        if(dataSize > 0) {
            memset(componentData->DataBuffer[componentIndex] + sizeof(Entity), 0, dataSize);
        }

        if(__isComponentInitialized) {

            for_children(property, Properties, component) {
                if(GetPropertyKind(property) != PropertyKind_Child) continue;

                auto child = __CreateEntity();
                __InjectChildPropertyValue(property, entity, child);
                auto childComponent = GetPropertyChildComponent(property);
                AddComponent(child, childComponent);
            }
            {
                for_children(base, Bases, component) {
                    AddComponent(entity, GetBaseComponent(base));
                }
            }

            auto t = GetComponentTemplate(component);
            if(IsEntityValid(t)) {
                auto name = GetDebugName(t);
                CopyEntity(t, entity);
            }

			Type types[] = { TypeOf_Entity, TypeOf_Entity };
			const void* values[] = { &component, &entity };
			FireEventFast(EventOf_EntityComponentAdded(), 2, types, values);
        }

        Verbose(VerboseLevel_ComponentEntityCreationDeletion, "Component %s has been added to Entity %s.", GetDebugName(component), GetDebugName(entity));

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
		Type types[] = { TypeOf_Entity, TypeOf_Entity };
		const void* values[] = { &component, &entity };
		FireEventFast(EventOf_EntityComponentRemoved(), 2, types, values);

        if(HasComponent(entity, component)) {
            auto componentData = GetComponentType(component);
            auto deletionEntityIndex = GetEntityIndex(entity);
            auto deletionComponentIndex = componentData->EntityComponentIndices[deletionEntityIndex];

            auto properties = GetProperties(component);
            for(auto i = 0; i < GetNumProperties(component); ++i) {
                auto property = properties[i];
                auto kind = GetPropertyKind(property);

                if(kind == PropertyKind_Child) {
                    Entity childEntity = 0;
                    GetPropertyValue(property, entity, &childEntity);
                    __DestroyEntity(childEntity);
                } else if(kind == PropertyKind_Array) {
                    while(GetArrayPropertyCount(property, entity)) {
                        RemoveArrayPropertyElement(property, entity, 0);
                    }
                } else {
                    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

                    SetPropertyValue(property, entity, nullData); // Reset value to default before removal
                }
            }

            componentData->EntityComponentIndices[deletionEntityIndex] = InvalidIndex;

            memset(componentData->DataBuffer[deletionComponentIndex], 0, componentData->DataBuffer.GetElementSize());

            componentData->DataBuffer.Remove(deletionComponentIndex);
        }

        Verbose(VerboseLevel_ComponentEntityCreationDeletion, "Component %s has been removed from Entity %s.", GetDebugName(component), GetDebugName(entity));

        return true;
    }
    return false;
}

API_EXPORT u32 GetNextComponent(Entity component, u32 index, void **dataPtr, Entity *entity) {
    ++index;

    auto componentData = GetComponentType(component);
    auto amount = componentData->DataBuffer.End();
    while(!componentData->DataBuffer.IsValid(index)) {
        ++index;

        if(index >= amount) {
            *dataPtr = 0;
            *entity = 0;
            return InvalidIndex;
        }
    }

    auto entryData = componentData->DataBuffer[index];
    *dataPtr = entryData + sizeof(Entity);
    *entity = *(Entity*)entryData;

    return index;
}

API_EXPORT void SetComponentSize(Entity entity, u16 value) {
    auto data = GetComponentData(entity);
    auto oldValue = 0;
    if(data) {
        oldValue = data->ComponentSize;
        data->ComponentSize = value;
    }

    auto componentData = GetComponentType(entity);
    componentData->DataBuffer.SetElementSize(value + sizeof(Entity));

    if(__isComponentInitialized) {
        Type types[] = {TypeOf_Entity, TypeOf_u16, TypeOf_u16};
        const void *args[] = {&entity, &oldValue, &value};
        FireEventFast(EventOf_ComponentSizeChanged(), 3, types, args);

        auto property = PropertyOf_ComponentSize();
        Type typesGeneric[] = {TypeOf_Entity, TypeOf_Entity};
        const void *argsGeneric[] = {&property, &entity};
        FireEventFast(EventOf_PropertyChanged(), 2, typesGeneric, argsGeneric);
    }
}

API_EXPORT u16 GetComponentSize(Entity entity) {
    if(HasComponent(entity, ComponentOf_Component())) {
        return GetComponentData(entity)->ComponentSize;
    }

    return 0;
}

__PropertyCoreImpl(bool, ComponentExplicitSize, Component)
__PropertyCoreImpl(Entity, ComponentTemplate, Component)
__ArrayPropertyCoreImpl(Property, Properties, Component)
__ArrayPropertyCoreImpl(Base, Bases, Component)

LocalFunction(OnEntityDestroyed, void, Entity entity) {
    for_entity(component, data, Component) {
        RemoveComponent(entity, component);
    }
}

static void RemoveExtensions(Entity component, Entity extensionComponent) {
    Entity entity = 0;
    void *data = 0;
    for(u32 i = GetNextComponent(component, InvalidIndex, &data, &entity);
        i != InvalidIndex;
        i = GetNextComponent(component, i, &data, &entity)) {
        RemoveComponent(entity, extensionComponent);
    }
}

static void AddExtensions(Entity component, Entity extensionComponent) {
    Entity entity = 0;
    void *data = 0;
    for(u32 i = GetNextComponent(component, InvalidIndex, &data, &entity);
        i != InvalidIndex;
        i = GetNextComponent(component, i, &data, &entity)) {
        AddComponent(entity, extensionComponent);
    }
}

LocalFunction(OnExtensionDisabledChanged,
        void,
        Entity extension,
        bool oldValue,
        bool newValue) {
    if(newValue) {
        AddExtensions(GetExtensionComponent(extension), GetExtensionExtenderComponent(extension));
    }

    if(!newValue) {
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
        RegisterChildProperty(Ownership, ComponentTemplate)
    EndComponent()

    BeginComponent(Extension)
        RegisterReferenceProperty(Component, ExtensionComponent)
        RegisterReferenceProperty(Component, ExtensionExtenderComponent)
        RegisterProperty(bool, ExtensionDisabled)
    EndComponent()

    RegisterEvent(EntityComponentAdded)
    RegisterEvent(EntityComponentRemoved)

    __isComponentInitialized = true;

    RegisterSubscription(EntityDestroyed, OnEntityDestroyed, 0)

    RegisterSubscription(ExtensionDisabledChanged, OnExtensionDisabledChanged, 0)
    RegisterSubscription(ExtensionComponentChanged, OnExtensionComponentChanged, 0)
    RegisterSubscription(ExtensionExtenderComponentChanged, OnExtensionExtenderComponentChanged, 0)
EndUnit()


void __InitializeComponent() {
    auto component = ComponentOf_Component();
    AddComponent(component, ComponentOf_Component());
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Properties(), offsetof(Component, Properties), sizeof(Component::Properties), TypeOf_Entity, component, ComponentOf_Property(), PropertyKind_Array);
    __Property(PropertyOf_ComponentSize(), offsetof(Component, ComponentSize), sizeof(Component::ComponentSize), TypeOf_u16, component, 0, PropertyKind_Value);
}
