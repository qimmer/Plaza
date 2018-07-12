//
// Created by Kim on 03/06/2018.
//

#ifndef PLAZA_COMPONENT_H
#define PLAZA_COMPONENT_H

#include <Core/NativeUtils.h>

#define InvalidIndex 0xFFFFFFFF

Function(AddComponent, bool, Entity entity, Entity componentType)
Function(RemoveComponent, bool, Entity entity, Entity componentType)
Function(HasComponent, bool, Entity entity, Entity componentType)

Function(GetNumComponents, u32, Entity component);
Function(GetComponentEntity, Entity, Entity component, u32 index);
Function(GetComponentIndex, u32, Entity entity, Entity component);

Unit(Component)
    Component(Component)
        Property(u16, ComponentSize)
        Property(bool, ComponentAbstract)
        Property(Entity, ComponentAddedEvent)
        Property(Entity, ComponentRemovedEvent)

    Component(Extension)
        Property(Entity, ExtensionComponent)

    Component(Base)
        Property(Entity, BaseComponent)

    Event(EntityComponentAdded)
    Event(EntityComponentRemoved)

char * GetComponentData(Entity component, u32 index);

u32 GetNextComponent(Entity component, u32 index, void **dataPtr, Entity *entity);

void __InitializeComponent();

#define for_entity(ENTITYVAR, DATAVAR, COMPONENTTYPE) \
    auto __component ## DATAVAR = ComponentOf_ ## COMPONENTTYPE ();\
    Entity ENTITYVAR = 0;\
    struct COMPONENTTYPE * DATAVAR = 0;\
    for(u32 __index = GetNextComponent(__component ## DATAVAR, InvalidIndex, (void**)&DATAVAR, &ENTITYVAR); __index != InvalidIndex; __index = GetNextComponent(__component ## DATAVAR, __index, (void**)&DATAVAR, &ENTITYVAR))

#endif //PLAZA_COMPONENT_H
