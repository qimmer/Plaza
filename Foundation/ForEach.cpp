//
// Created by Kim on 25-02-2019.
//

#include "ForEach.h"
#include <Core/Property.h>
#include <Core/Instance.h>

#include <EASTL/map.h>

static eastl::map<Entity, Vector<Entity, 4>> entityForEachListeners;

static void ReplicateChildren(Entity forEach, ForEach *forEachData, Entity sourceProperty) {
	for_children_abstract(sourceChild, sourceProperty, forEachData->ForEachSourceEntity) {
		auto instanceChildIndex = AddArrayPropertyElement(forEachData->ForEachDestinationArrayProperty, forEachData->ForEachDestinationEntity);
		auto instanceChild = GetArrayPropertyElement(forEachData->ForEachDestinationArrayProperty, forEachData->ForEachDestinationEntity, instanceChildIndex);

		SetForEach(instanceChild, forEach);
		SetForEachSource(instanceChild, sourceChild);

		SetInstanceTemplate(instanceChild, forEachData->ForEachTemplate);
	}
}
static void ForEachChanged(
        Entity forEach,
        ForEach *forEachData,
        Entity oldDestinationProperty,
        Entity oldDestination,
        Entity oldSource
) {
    if(oldSource) {
        eastl::remove(entityForEachListeners[oldSource].begin(), entityForEachListeners[oldSource].end(), forEach);

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
        && forEachData->ForEachSourceEntity
        && forEachData->ForEachDestinationArrayProperty
        && forEachData->ForEachDestinationEntity
        && forEachData->ForEachTemplate) {
		
        entityForEachListeners[forEachData->ForEachSourceEntity].push_back(forEach);

        // Replicate children
		if (forEachData->ForEachSourceArrayProperty) {
			ReplicateChildren(forEach, forEachData, forEachData->ForEachSourceArrayProperty);
		}
		else {
			auto& components = GetEntityComponents(forEachData->ForEachSourceEntity);

			for (auto it = components.begin(); it != components.end(); ++it) {
				auto& component = it->first;

				for_children(property, Properties, component) {
					ReplicateChildren(forEach, forEachData, property);
				}
			}
			
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

LocalFunction(OnRemoved, void, Entity component, Entity forEach) {
	auto forEachData = GetForEachData(forEach);
	forEachData->ForEachEnabled = false;

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

	RegisterSubscription(EventOf_EntityComponentRemoved(), OnRemoved, ComponentOf_ForEach())
EndUnit()
