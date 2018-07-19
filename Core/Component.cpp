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

static Vector<ComponentTypeData> ComponentDataList;
static Vector<u16> ComponentDataIndices;

struct Extension {
    Entity ExtensionComponent;
};

struct Base {
    Entity BaseComponent;
};

struct Component {
    Vector(Properties, Entity, 32)
    Vector(Bases, Entity, 8)
    u16 ComponentDataIndex;
    u16 ComponentSize;
};

static ComponentTypeData *GetComponentType(Entity component) {
    auto entityIndex = GetEntityIndex(component);
    if(entityIndex >= ComponentDataIndices.size()) {
        ComponentDataIndices.resize(upper_power_of_two(Max(entityIndex + 1, 8)), UINT16_MAX);
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

API_EXPORT char * GetComponentData (Entity component, u32 index) {
    if(index == InvalidIndex || !IsEntityValid(component)) return NULL;

    auto componentData = GetComponentType(component);
    return componentData->DataBuffer[index] + sizeof(Entity);
}

API_EXPORT bool AddComponent (Entity entity, Entity component) {
    Assert(entity, IsEntityValid(entity) && IsEntityValid(component));

    if(!HasComponent (entity, component)) {
        auto componentData = GetComponentType(component);

        auto componentIndex = componentData->DataBuffer.Add();
        auto entityIndex = GetEntityIndex(entity);

        if(componentData->EntityComponentIndices.size() <= entityIndex) {
            componentData->EntityComponentIndices.resize(entityIndex + 1, InvalidIndex);
        }

        componentData->EntityComponentIndices[entityIndex] = componentIndex;
        *(Entity*)componentData->DataBuffer[componentIndex] = entity;

        memset(componentData->DataBuffer[componentIndex] + sizeof(Entity), 0, componentData->DataBuffer.GetElementSize() - sizeof(Entity));

        if(__isComponentInitialized) {
            auto bases = GetBases(component);
            for(auto i = 0; i < GetNumBases(component); ++i) {
                auto base = bases[i];
                auto dependency = GetBaseComponent(base);
                AddComponent(entity, dependency);
            }

            FireEvent(EventOf_EntityComponentAdded(), component, entity);
        }

        Verbose("Component %llu has been added to Entity %llu.", component, entity);

        return true;
    }
    return false;
}

API_EXPORT bool RemoveComponent (Entity entity, Entity component) {
    Assert(entity, IsEntityValid(entity));
    Assert(component, IsEntityValid(component));

    if(HasComponent(entity, component)) {
        Verbose("Removing Component %llu from Entity %llu ...", component, entity);

        FireEvent(EventOf_EntityComponentRemoved(), component, entity);

        if(HasComponent(entity, component)) {
            auto componentData = GetComponentType(component);
            auto deletionEntityIndex = GetEntityIndex(entity);
            auto deletionComponentIndex = componentData->EntityComponentIndices[deletionEntityIndex];

            componentData->EntityComponentIndices[deletionEntityIndex] = InvalidIndex;

            memset(componentData->DataBuffer[deletionComponentIndex], 0, componentData->DataBuffer.GetElementSize());

            componentData->DataBuffer.Remove(deletionComponentIndex);
        }

        Verbose("Component %s has been removed from Entity %s.", GetName(component), GetName(entity));

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
    if(HasComponent(entity, ComponentOf_Component())) {
        GetComponentData(entity)->ComponentSize = value;
    }

    auto componentData = GetComponentType(entity);
    componentData->DataBuffer.SetElementSize(value + sizeof(Entity));
}

__ArrayPropertyCoreImpl(Base, Bases, Component)
__ArrayPropertyCoreImpl(Property, Properties, Component)

LocalFunction(OnEntityDestroyed, void, Entity entity) {
    for_entity(component, data, Component) {
        RemoveComponent(entity, component);
    }
}

BeginUnit(Component)
    BeginComponent(Component)
        RegisterArrayProperty(Property, Properties)
        RegisterArrayProperty(Base, Bases)
        RegisterProperty(u16, ComponentSize)
    EndComponent()

    BeginComponent(Extension)
        RegisterProperty(Entity, ExtensionComponent)
    EndComponent()

    BeginComponent(Base)
        RegisterProperty(Entity, BaseComponent)
    EndComponent()

    RegisterEvent(EntityComponentAdded)
    RegisterEvent(EntityComponentRemoved)

    __isComponentInitialized = true;

    RegisterSubscription(EntityDestroyed, OnEntityDestroyed, 0)
EndUnit()


void __InitializeComponent() {
    auto component = ComponentOf_Component();
    AddComponent(component, ComponentOf_Component());
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Bases(), offsetof(Component, Bases), sizeof(Component::Bases), TypeOf_Entity, component, ComponentOf_Base(), PropertyKind_Array);
    __Property(PropertyOf_Properties(), offsetof(Component, Properties), sizeof(Component::Properties), TypeOf_Entity, component, ComponentOf_Property(), PropertyKind_Array);
    __Property(PropertyOf_ComponentSize(), offsetof(Component, ComponentSize), sizeof(Component::ComponentSize), TypeOf_u16, component, 0, PropertyKind_Value);
}
