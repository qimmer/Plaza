//
// Created by Kim on 25-02-2019.
//

#include "ForEach.h"
#include <Core/Property.h>
#include <Core/Instance.h>

LocalFunction(OnForEachChanged, void, Entity forEach) {

}

LocalFunction(OnChanged, void, Entity property, Entity entity, Type valueType, Variant oldValue, Variant newValue) {

}

BeginUnit(ForEach)
    BeginComponent(ForEach)
        RegisterProperty(Entity, ForEachSourceEntity)
        RegisterProperty(Entity, ForEachDestinationEntity)
        RegisterReferenceProperty(Property, ForEachSourceArrayProperty)
        RegisterReferenceProperty(Property, ForEachDestinationArrayProperty)
        RegisterChildProperty(Ownership, ForEachTemplate)
    EndComponent()

    BeginComponent(ForEachInstance)
        RegisterPropertyReadOnly(Entity, ForEachModel)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachSourceEntity()), OnForEachChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachDestinationEntity()), OnForEachChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachSourceArrayProperty()), OnForEachChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachDestinationArrayProperty()), OnForEachChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachTemplate()), OnForEachChanged, 0)
EndUnit()
