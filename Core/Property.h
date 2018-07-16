//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_PROPERTY_H
#define PLAZA_PROPERTY_H

#include <Core/NativeUtils.h>

#define PropertyFlag_ReadOnly 1
#define PropertyFlag_Transient 2

Unit(Property)

__Component(Property)
    Property(u32, PropertyOffset)
    Property(u32, PropertySize)
    Property(Type, PropertyType)
    Property(Entity, PropertyEnum)
    Property(u32, PropertyFlags)
    Property(Entity, PropertyComponent)
__PropertyNoInit(Entity, PropertyChangedEvent)

Component(Binding)
    Property(Entity, BindingSourceEntity)
    Property(Entity, BindingSourceProperty)
    Property(Entity, BindingTargetEntity)
    Property(Entity, BindingTargetProperty)

void SetPropertyValue(Entity entity, Entity context, const void *valueData);
bool GetPropertyValue(Entity entity, Entity context, void *dataOut);

void SetPropertyMeta(Entity property, StringRef metaString);

void __Property(Entity property, u32 offset, u32 size, Type type, Entity component);
void __InitializeProperty();

#endif //PLAZA_PROPERTY_H
