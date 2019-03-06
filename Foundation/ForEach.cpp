//
// Created by Kim on 25-02-2019.
//

#include "ForEach.h"
#include <Core/Property.h>
#include <Core/Instance.h>

static eastl::vector<Vector<Entity, 4>> entityForEachListeners;

static void ForEachChanged(
        Entity forEach,
        ForEach *forEachData,
        Entity oldDestinationProperty,
        Entity oldDestination,
        Entity oldSource
) {
    if(oldSource) {
        auto entityIndex = GetEntityIndex(oldSource);

        if(entityForEachListeners.size() <= entityIndex) {
            entityForEachListeners.resize(entityIndex + 1);
        }

        eastl::remove(entityForEachListeners[entityIndex].begin(), entityForEachListeners[entityIndex].end(), forEach);

        // Remove replicated children
        Vector<Entity, 32> childrenToDestroy;

        for_children_abstract(instance, oldDestinationProperty, oldDestination) {
            if(GetForEach(instance) == forEach) {
                childrenToDestroy.push_back(instance);
            }
        }

        for(auto& child : childrenToDestroy) {
            auto index = GetArrayPropertyIndex(oldDestinationProperty, oldDestination, child);
            RemoveArrayPropertyElement(oldDestinationProperty, oldDestination, index);
        }
    }

    if(forEachData->ForEachEnabled
        && forEachData->ForEachSourceArrayProperty
        && forEachData->ForEachSourceEntity
        && forEachData->ForEachDestinationArrayProperty
        && forEachData->ForEachDestinationEntity
        && forEachData->ForEachTemplate) {

        auto entityIndex = GetEntityIndex(forEachData->ForEachSourceEntity);

        if(entityForEachListeners.size() <= entityIndex) {
            entityForEachListeners.resize(entityIndex + 1);
        }

        entityForEachListeners[entityIndex].push_back(forEach);

        // Replicate children
        for_children_abstract(sourceChild, forEachData->ForEachSourceArrayProperty, forEachData->ForEachSourceEntity) {
            auto instanceChildIndex = AddArrayPropertyElement(forEachData->ForEachDestinationArrayProperty, forEachData->ForEachDestinationEntity);
            auto instanceChild = GetArrayPropertyElement(forEachData->ForEachDestinationArrayProperty, forEachData->ForEachDestinationEntity, instanceChildIndex);

            SetForEach(instanceChild, forEach);
            SetForEachSource(instanceChild, sourceChild);

            SetInstanceTemplate(instanceChild, forEachData->ForEachTemplate);
        }
    }
}

LocalFunction(OnForEachSourceEntityChanged, void, Entity forEach, Entity oldValue, Entity newValue) {
    auto forEachData = GetForEachData(forEach);
    ForEachChanged(forEach, forEachData, forEachData->ForEachDestinationArrayProperty, forEachData->ForEachDestinationEntity, oldValue);
}

LocalFunction(OnForEachDestinationEntityChanged, void, Entity forEach, Entity oldValue, Entity newValue) {
    auto forEachData = GetForEachData(forEach);
    ForEachChanged(forEach, forEachData, forEachData->ForEachDestinationArrayProperty, oldValue, forEachData->ForEachSourceEntity);
}

LocalFunction(OnForEachSourceArrayPropertyChanged, void, Entity forEach, Entity oldValue, Entity newValue) {
    auto forEachData = GetForEachData(forEach);
    ForEachChanged(forEach, forEachData, forEachData->ForEachDestinationArrayProperty, forEachData->ForEachDestinationEntity, forEachData->ForEachSourceEntity);
}

LocalFunction(OnForEachDestinationArrayPropertyChanged, void, Entity forEach, Entity oldValue, Entity newValue) {
    auto forEachData = GetForEachData(forEach);
    ForEachChanged(forEach, forEachData, oldValue, forEachData->ForEachDestinationEntity, forEachData->ForEachSourceEntity);
}

LocalFunction(OnForEachTemplateChanged, void, Entity forEach, Entity oldValue, Entity newValue) {
    auto forEachData = GetForEachData(forEach);
    ForEachChanged(forEach, forEachData, forEachData->ForEachDestinationArrayProperty, forEachData->ForEachDestinationEntity, forEachData->ForEachSourceEntity);
}

LocalFunction(OnForEachEnabledChanged, void, Entity forEach, Entity oldValue, Entity newValue) {
    auto forEachData = GetForEachData(forEach);
    ForEachChanged(forEach, forEachData, forEachData->ForEachDestinationArrayProperty, forEachData->ForEachDestinationEntity, forEachData->ForEachSourceEntity);
}

BeginUnit(ForEach)
    BeginComponent(ForEach)
        RegisterProperty(bool, ForEachEnabled)
        RegisterProperty(Entity, ForEachSourceEntity)
        RegisterProperty(Entity, ForEachDestinationEntity)
        RegisterReferenceProperty(Property, ForEachSourceArrayProperty)
        RegisterReferenceProperty(Property, ForEachDestinationArrayProperty)
        RegisterProperty(Entity, ForEachTemplate)
    EndComponent()

    BeginComponent(ForEachInstance)
        RegisterPropertyReadOnly(Entity, ForEach)
        RegisterPropertyReadOnly(Entity, ForEachSource)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachSourceEntity()), OnForEachSourceEntityChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachDestinationEntity()), OnForEachDestinationEntityChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachSourceArrayProperty()), OnForEachSourceArrayPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachDestinationArrayProperty()), OnForEachDestinationArrayPropertyChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachTemplate()), OnForEachTemplateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_ForEachEnabled()), OnForEachEnabledChanged, 0)
EndUnit()
