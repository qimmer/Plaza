//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_PROPERTY_H
#define PLAZA_PROPERTY_H

#include <Core/NativeUtils.h>

#define PropertyFlag_ReadOnly 1
#define PropertyFlag_Transient 2

#define PropertyKind_Value 1
#define PropertyKind_String 2
#define PropertyKind_Child 3
#define PropertyKind_Array 4

Unit(Property)

Component(Property)
    __PropertyCore(Property, u32, PropertyOffset)
    __PropertyCore(Property, u32, PropertySize)
    __PropertyCore(Property, Type, PropertyType)
    __PropertyCore(Property, u8, PropertyKind)
    __PropertyCore(Property, Entity, PropertyChildComponent)
    __PropertyCore(Property, Entity, PropertyEnum)
    ChildProperty(Event, PropertyChangedEvent)
    __PropertyCore(Property, u32, PropertyFlags)

Component(Ownership)
    __PropertyCoreGetOnly(Ownership, Entity, Owner, PropertyFlag_ReadOnly, PropertyFlag_Transient)
    __PropertyCoreGetOnly(Ownership, Entity, OwnerProperty, PropertyFlag_ReadOnly, PropertyFlag_Transient)
    void SetOwner(Entity entity, Entity owner, Entity ownerProperty);

Component(Binding)
    Property(Entity, BindingSourceEntity)
    Property(Entity, BindingSourceProperty)
    Property(Entity, BindingTargetEntity)
    Property(Entity, BindingTargetProperty)

Event(PropertyChanged, Entity property, Entity context)

void SetPropertyValue(Entity property, Entity entity, const void *valueData);
bool GetPropertyValue(Entity property, Entity entity, void *dataOut);
Entity *GetArrayPropertyElements(Entity property, Entity entity);

Function(GetArrayPropertyCount, u32, Entity property, Entity entity);
Function(AddArrayPropertyElement, u32, Entity property, Entity entity);
Function(RemoveArrayPropertyElement, bool, Entity property, Entity entity, u32 index);
Function(GetArrayPropertyElement, Entity, Entity property, Entity entity, u32 index);

u32 __InjectArrayPropertyElement(Entity property, Entity entity, Entity element);
void __InjectChildPropertyValue(Entity property, Entity entity, Entity value);

void SetPropertyMeta(Entity property, StringRef metaString);

void __Property(Entity property, u32 offset, u32 size, Type type, Entity component, Entity childComponent, u8 kind);
void __InitializeProperty();

void AddElementFromDecl(Entity property, Entity module, StringRef decl);
void SetChildFromDecl(Entity property, Entity module, StringRef decl);

#endif //PLAZA_PROPERTY_H
