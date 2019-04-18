//
// Created by Kim on 25-02-2019.
//

#include "ForEach.h"
#include <Core/Property.h>
#include <Core/Instance.h>

#include <EASTL/map.h>

static eastl::map<Entity, eastl::fixed_vector<Entity, 4>> entityForEachListeners;

static void ReplicateChildren(Entity forEach, const ForEach& forEachData, Entity sourceProperty) {
    auto sourceArray = GetPropertyValue(sourceProperty, forEachData.ForEachSourceEntity).as_ChildArray;

	for(auto sourceChild : sourceArray) {
		auto instanceChild = CreateEntity();
		SetForEachInstance(instanceChild, {sourceChild, forEach});

        auto destinationArray = GetPropertyValue(forEachData.ForEachDestinationArrayProperty, forEachData.ForEachDestinationEntity);
        destinationArray.as_ChildArray.Add(instanceChild);
        SetPropertyValue(forEachData.ForEachDestinationArrayProperty, forEachData.ForEachDestinationEntity, destinationArray);

		SetInstance(instanceChild, {forEachData.ForEachTemplate});
	}
}
static void OnForEachChanged(
        Entity forEach,
        const ForEach& forEachData,
        const ForEach& oldForachData
) {
    if(oldForachData.ForEachSourceEntity) {
        eastl::remove(entityForEachListeners[oldForachData.ForEachSourceEntity].begin(), entityForEachListeners[oldForachData.ForEachSourceEntity].end(), forEach);

        // Remove replicated children
        eastl::fixed_vector<Entity, 32> childrenToDestroy;

        auto destinationArray = GetPropertyValue(oldForachData.ForEachDestinationArrayProperty, oldForachData.ForEachDestinationEntity);
        destinationArray.as_ChildArray.SetSize(0);
        SetPropertyValue(oldForachData.ForEachDestinationArrayProperty, oldForachData.ForEachDestinationEntity, destinationArray);
    }

    if(forEachData.ForEachEnabled
        && forEachData.ForEachSourceEntity
        && forEachData.ForEachDestinationArrayProperty
        && forEachData.ForEachDestinationEntity
        && forEachData.ForEachTemplate) {
		
        entityForEachListeners[forEachData.ForEachSourceEntity].push_back(forEach);

        // Replicate children
		if (forEachData.ForEachSourceArrayProperty) {
			ReplicateChildren(forEach, forEachData, forEachData.ForEachSourceArrayProperty);
		}
		else {
		    Component componentData;
			for_entity_data(component, ComponentOf_Component(), &componentData) {
				if (!HasComponent(forEachData.ForEachSourceEntity, component)) continue;

				for(auto property : componentData.Properties) {
					ReplicateChildren(forEach, forEachData, property);
				}
			}
			
		}
    }
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
        RegisterPropertyReadOnly(Entity, ForEachInstanceForEach)
        RegisterPropertyReadOnly(Entity, ForEachInstanceSource)
    EndComponent()

    RegisterSystem(OnForEachChanged, ComponentOf_ForEach())
EndUnit()
