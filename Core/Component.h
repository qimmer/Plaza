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

Function(GetComponentMax, u32, Entity component);
Function(GetComponentEntity, Entity, Entity component, u32 index);
Function(GetComponentIndex, u32, Entity entity, Entity component);

Unit(Component)
    Component(Extension)
        Property(Entity, ExtensionComponent)
        Property(Entity, ExtensionExtenderComponent)
        Property(bool, ExtensionDisabled)

    Component(Base)
        Property(Entity, BaseComponent)

    Component(Component)
        __PropertyCore(Component, u16, ComponentSize)
        __ArrayPropertyCore(Base, Bases)
        __ArrayPropertyCore(Property, Properties)

    Event(EntityComponentAdded, Entity entity)
    Event(EntityComponentRemoved, Entity entity)

char * GetComponentBytes(Entity component, u32 index);

u32 GetNextComponent(Entity component, u32 index, void **dataPtr, Entity *entity);

void __InitializeComponent();

#define for_entity(ENTITYVAR, DATAVAR, COMPONENTTYPE) \
    auto __component ## DATAVAR = ComponentOf_ ## COMPONENTTYPE ();\
    Entity ENTITYVAR = 0;\
    struct COMPONENTTYPE * DATAVAR = 0;\
    for(u32 __index ## ENTITYVAR = GetNextComponent(__component ## DATAVAR, InvalidIndex, (void**)&DATAVAR, &ENTITYVAR); __index ## ENTITYVAR != InvalidIndex; __index ## ENTITYVAR = GetNextComponent(__component ## DATAVAR, __index ## ENTITYVAR, (void**)&DATAVAR, &ENTITYVAR))

#endif //PLAZA_COMPONENT_H
