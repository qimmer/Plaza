//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_PROPERTY_H
#define PLAZA_PROPERTY_H

#include <Core/NativeUtils.h>

#define PropertyKind_Value 0
#define PropertyKind_Child 1
#define PropertyKind_Array 2

struct ArrayChild {
    StringRef Name;
};

struct Ownership {
    Entity Owner, OwnerProperty;
};

struct Property {
    Entity PropertyEnum, PropertyChangedEvent, PropertyChildComponent;
    u32 PropertyOffset, PropertySize, PropertyFlags;
    Type PropertyType;
    u8 PropertyKind;
    bool PropertyReadOnly;
};

void EmitChangedEvent(Entity entity, Entity property, Property *propertyData, Variant oldValueData, Variant newValueData);

#include <Core/Component.h>

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
    __PropertyCore(Property, Entity, PropertyChangedEvent)

Component(Ownership)
    __PropertyCoreGetOnly(Ownership, Entity, Owner, PropertyFlag_ReadOnly, PropertyFlag_Transient)
    __PropertyCoreGetOnly(Ownership, Entity, OwnerProperty, PropertyFlag_ReadOnly, PropertyFlag_Transient)

Component(ArrayChild)
    __PropertyCore(ArrayChild, StringRef, Name)

Function(IsOwnedBy, bool, Entity entity, Entity owner)
    void SetOwner(Entity entity, Entity owner, Entity ownerProperty);

StringRef Intern(StringRef sourceString);

Function(SetPropertyValue, void, Entity property, Entity entity, Variant valueData)
Function(GetPropertyValue, Variant, Entity property, Entity entity)

const ChildArray& GetArrayPropertyElements(Entity property, Entity entity);

Function(GetArrayPropertyCount, u32, Entity property, Entity entity);
Function(AddArrayPropertyElement, u32, Entity property, Entity entity);
Function(RemoveArrayPropertyElement, bool, Entity property, Entity entity, u32 index);
Function(GetArrayPropertyIndex, u32, Entity property, Entity entity, Entity element);
Function(SetArrayPropertyCount, bool, Entity property, Entity entity, u32 count);
Function(GetArrayPropertyElement, Entity, Entity property, Entity entity, u32 index);

u32 AddChild(Entity property, Entity entity, Entity child, bool takeOwnership);
bool RemoveChild(Entity property, Entity entity, u32 index);
const ChildArray& GetChildArray(Entity property, Entity entity);
u32 GetChildIndex(Entity property, Entity entity, Entity child);

void SetPropertyMeta(Entity property, StringRef metaString);

void __Property(Entity property, u32 offset, u32 size, Type type, Entity component, Entity childComponent, u8 kind, StringRef name);
void __InitializeProperty();
void __InitializeString();

void AddElementFromDecl(Entity property, Entity module, StringRef decl);
void SetChildFromDecl(Entity property, Entity module, StringRef decl);

Function(MergeArray, void, Entity property, Entity left, Entity right, Entity destination)
Function(ConcatArray, void, Entity property, Entity source, Entity destination)
Function(ClearArray, void, Entity property, Entity entity)
Function(MirrorArray, void, Entity property, Entity source, Entity destination)

void DumpTree(Entity entity);


typedef void(*GenericPropertyChangedListener)(Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue);

void RegisterGenericPropertyChangedListener(GenericPropertyChangedListener listener);
Vector<GenericPropertyChangedListener, 64>& GetGenericPropertyChangedListeners();

#endif //PLAZA_PROPERTY_H
