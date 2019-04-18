//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_PROPERTY_H
#define PLAZA_PROPERTY_H

#include <Core/NativeUtils.h>

struct ArrayChild {
    StringRef Name;
};

struct Ownership {
    Entity Owner, OwnerProperty;
    u16 OwnerLevel;
};

struct Property {
    Entity PropertyEnum, PropertyPrefab;
    u32 PropertyOffset, PropertySize;
    Type PropertyType;
    bool PropertyReadOnly;
};

#include <Core/Component.h>

Unit(Property)

Component(Property)
    Property(u32, PropertyOffset)
    Property(u32, PropertySize)
    Property(Type, PropertyType)
    Property(bool, PropertyReadOnly)
    Property(Entity, PropertyPrefab)
    Property(Entity, PropertyEnum)

Component(Ownership)
    Property(Entity, Owner)
    Property(Entity, OwnerProperty)
    Property(u16, OwnerLevel)

Component(ArrayChild)
    Property(StringRef, Name)

Function(IsOwnedBy, bool, Entity entity, Entity owner)

Function(SetPropertyValue, void, Entity property, Entity entity, Variant value);
Function(GetPropertyValue, Variant, Entity property, Entity entity);

StringRef Intern(StringRef sourceString);

Variant GetPropertyValue(Entity property, Entity entity);
void SetPropertyValue(Entity property, Entity entity, Variant value);

void DumpTree(Entity entity);

#endif //PLAZA_PROPERTY_H
