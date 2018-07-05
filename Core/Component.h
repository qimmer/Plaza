//
// Created by Kim on 03/06/2018.
//

#ifndef PLAZA_COMPONENT_H
#define PLAZA_COMPONENT_H

#include <Core/NativeUtils.h>

#define InvalidIndex 0xFFFFFFFF

bool AddComponent(Entity entity, Entity componentType);
bool RemoveComponent(Entity entity, Entity componentType);
bool HasComponent(Entity entity, Entity componentType);

u32 GetNumComponents(Entity component);
Entity GetComponentEntity(Entity component, u32 index);
char * GetComponentData(Entity component, u32 index);
u32 GetComponentIndex(Entity entity, Entity component);

Unit(Component)

Component(Component)
    Property(u16, ComponentSize)
    Property(bool, ComponentAbstract)
    Property(Entity, ComponentAddedEvent)
    Property(Entity, ComponentRemovedEvent)

Component(Extension)
    Property(Entity, ExtensionComponent)

Component(Dependency)
    Property(Entity, DependencyComponent)

struct EntityComponentAddedArgs {
    Entity ComponentEntity;
    Entity Component;
};

struct EntityComponentRemovedArgs {
    Entity ComponentEntity;
    Entity Component;
};

Event(EntityComponentAdded)
Event(EntityComponentRemoved)

void __InitializeComponent();

#endif //PLAZA_COMPONENT_H
