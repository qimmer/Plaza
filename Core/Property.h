//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_PROPERTY_H
#define PLAZA_PROPERTY_H

#include <Core/NativeUtils.h>

#define PropertyKind_Value 0
#define PropertyKind_Child 1
#define PropertyKind_Array 2

Unit(Property)

Enum(PropertyKind)

Component(Property)
    __PropertyCore(Property, u32, PropertyOffset)
    __PropertyCore(Property, u32, PropertySize)
    __PropertyCore(Property, Type, PropertyType)
    __PropertyCore(Property, u8, PropertyKind)
    __PropertyCore(Property, bool, PropertyReadOnly)
    __PropertyCore(Property, Entity, PropertyChildComponent)
    __PropertyCore(Property, Entity, PropertyEnum)
    ChildProperty(Event, PropertyChangedEvent)

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

StringRef Intern(StringRef sourceString);

void SetPropertyValue(Entity property, Entity entity, const void *valueData);
bool GetPropertyValue(Entity property, Entity entity, void *dataOut);
Entity *GetArrayPropertyElements(Entity property, Entity entity);

Function(GetArrayPropertyCount, u32, Entity property, Entity entity);
Function(AddArrayPropertyElement, u32, Entity property, Entity entity);
Function(RemoveArrayPropertyElement, bool, Entity property, Entity entity, u32 index);
Function(SetArrayPropertyCount, bool, Entity property, Entity entity, u32 count);
Function(GetArrayPropertyElement, Entity, Entity property, Entity entity, u32 index);

u32 __InjectArrayPropertyElement(Entity property, Entity entity, Entity element);
void __InjectChildPropertyValue(Entity property, Entity entity, Entity value);

void SetPropertyMeta(Entity property, StringRef metaString);

void __Property(Entity property, u32 offset, u32 size, Type type, Entity component, Entity childComponent, u8 kind);
void __InitializeProperty();
void __InitializeString();

void AddElementFromDecl(Entity property, Entity module, StringRef decl);
void SetChildFromDecl(Entity property, Entity module, StringRef decl);

Function(CopyEntity, void, Entity source, Entity destination, bool removeRedundantComponents = true)
Function(MergeArray, void, Entity property, Entity left, Entity right, Entity destination)
Function(ConcatArray, void, Entity property, Entity source, Entity destination)
Function(ClearArray, void, Entity property, Entity entity)
Function(MirrorArray, void, Entity property, Entity source, Entity destination)

#endif //PLAZA_PROPERTY_H
