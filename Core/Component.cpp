//
// Created by Kim on 03/06/2018.
//

#include <Core/Component.h>
#include <Core/Function.h>
#include <Core/Entity.h>
#include <Core/Event.h>
#include <Core/Pool.h>
#include <Core/Hierarchy.h>

struct Component {
    u16 ComponentSize;
    bool ComponentAbstract;
    Entity ComponentAddedEvent, ComponentRemovedEvent;
};

struct Extension {
    Entity ExtensionComponent;
};

struct Dependency {
    Entity DependencyComponent;
};

struct ComponentTypeData {
    ComponentTypeData() {
        DataBuffer.SetElementSize(64);
    }

    Pool DataBuffer;
    Vector<u32> EntityComponentIndices;
};

static Vector<ComponentTypeData> componentTypeList;

static inline ComponentTypeData *GetComponentType(Entity component) {
    auto index = GetEntityIndex(component);
    if(index >= componentTypeList.size()) {
        componentTypeList.resize(index + 1);
    }

    return &componentTypeList[index];
}

API_EXPORT u32 GetNumComponents (Entity component) {
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
    if(index == InvalidIndex) return NULL;

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

        auto componentSize = GetComponentSize(component);
        memset(componentData->DataBuffer[componentIndex] + sizeof(Entity), 0, componentSize);

        for_children(child, component) {
            if(HasComponent(child, ComponentOf_Dependency())) {
                auto dependency = GetDependencyComponent(child);
                AddComponent(entity, dependency);
            }
        }

        FireEntityComponentAdded({entity, component});

        const void *arguments[] = {&entity};
        const Type argumentTypes[] = {TypeOf_Entity};
        FireEventFast(GetComponentAddedEvent(component), 1, argumentTypes, arguments);

        Verbose("Component %s has been added to Entity %s.", GetName(component), GetName(entity));

        return true;
    }
    return false;
}

API_EXPORT bool RemoveComponent (Entity entity, Entity component) {
    Assert(entity, IsEntityValid(entity));
    Assert(component, IsEntityValid(component));

    if(HasComponent(entity, component)) {
        Verbose("Removing Component %s from Entity %s ...", GetName(component), GetName(entity));

        const void *arguments[] = {&entity};
        const Type argumentTypes[] = {TypeOf_Entity};
        FireEventFast(GetComponentRemovedEvent(component), 1, argumentTypes, arguments);

        FireEntityComponentRemoved({entity, component});

        if(HasComponent(entity, component)) {
            auto componentData = GetComponentType(component);
            auto deletionEntityIndex = GetEntityIndex(entity);
            auto deletionComponentIndex = componentData->EntityComponentIndices[deletionEntityIndex];

            componentData->EntityComponentIndices[deletionEntityIndex] = InvalidIndex;
            componentData->DataBuffer.Remove(deletionComponentIndex);
        }

        Verbose("Component %s has been removed from Entity %s.", GetName(component), GetName(entity));

        return true;
    }
    return false;
}

LocalFunction(OnEntityDestroyed, void, Entity entity) {
    auto numComponents = GetNumComponents(ComponentOf_Component());
    for(auto i = 0; i < numComponents; ++i ) {
        auto component = GetComponentEntity(ComponentOf_Component(), i);
        if(!component) continue;

        auto name = GetName(component);
        RemoveComponent(entity, component);
    }
}

BeginUnit(Component)
    RegisterSubscription(EntityDestroyed, OnEntityDestroyed)

    BeginComponent(Component)
        RegisterProperty(u16, ComponentSize)
        RegisterProperty(bool, ComponentAbstract)
        RegisterProperty(Entity, ComponentAddedEvent)
        RegisterProperty(Entity, ComponentRemovedEvent)
    EndComponent()

    BeginComponent(Extension)
        RegisterProperty(Entity, ExtensionComponent)
    EndComponent()

    BeginComponent(Dependency)
        RegisterProperty(Entity, DependencyComponent)
    EndComponent()
EndUnit()


void __InitializeComponent() {
    auto component = ComponentOf_Component();
    __Property(PropertyOf_ComponentSize(), offsetof(Component, ComponentSize), sizeof(Component::ComponentSize), TypeOf_u16,  component);
}