//
// Created by Kim on 28-07-2018.
//

#include <Json/NativeUtils.h>
#include <Foundation/Stream.h>
#include "EntityTracker.h"
#include "DebugModule.h"

struct EntityModification {
    Entity EntityModificationEntity;
};

struct EntityTracker {
    Vector(EntityModifications, Entity, 128);
    bool EntityTrackerTrackValues;
};

static Entity TriggerChange(Entity tracker, Entity changedEntity) {
    auto trackerData = GetEntityTrackerData(tracker);

    for(auto i = 0; i < trackerData->EntityModifications.Count; ++i) {
        auto modification = GetVector(trackerData->EntityModifications)[i];
        auto modificationData = GetEntityModificationData(modification);

        if(modificationData && modificationData->EntityModificationEntity == changedEntity) {
            return modification;
        }
    }

    auto modification = AddEntityModifications(tracker);
    SetEntityModificationEntity(modification, changedEntity);

    return modification;
}

LocalFunction(OnEntityDestroyed, void, Entity entity) {
    for_entity(tracker, trackerData, EntityTracker) {
        auto trackerData = GetEntityTrackerData(tracker);

        for(auto i = 0; i < trackerData->EntityModifications.Count; ++i) {
            auto modification = GetVector(trackerData->EntityModifications)[i];
            auto modificationData = GetEntityModificationData(modification);

            if(modificationData && modificationData->EntityModificationEntity == entity) {
                RemoveEntityModifications(tracker, i);
                --i;
            }
        }
    }
}

LocalFunction(OnPropertyChanged, void, Entity property, Entity entity) {
    if(property == PropertyOf_EntityModificationEntity() || property == PropertyOf_EntityModifications()) return;

    auto kind = GetPropertyKind(property);
    if(kind == PropertyKind_Array) {
        for_entity(tracker, trackerData, EntityTracker) {
            TriggerChange(tracker, entity);
        }
    }
    else if(kind == PropertyKind_Child) {
        for_entity(tracker, trackerData, EntityTracker) {
            TriggerChange(tracker, entity);
        }
    } else {
        for_entity(tracker, trackerData, EntityTracker) {
            if(trackerData->EntityTrackerTrackValues) {
                TriggerChange(tracker, entity);
            }
        }
    }
}

API_EXPORT u16 GetChanges(Entity responseStream) {
    SetStreamPath(responseStream, "memory://response.json");

    auto entityTracker = GetEntityTracker(ModuleOf_Debug());

    if(!SerializeJson(
            responseStream,
            entityTracker,
            4, // Include changed entity's children, but not sub-children
            2)) { // Include entity tracker's modifications' entities
        return 500;
    }

    while(GetNumEntityModifications(entityTracker)) {
        RemoveEntityModifications(entityTracker, 0);
    }

    return 200;
}

BeginUnit(EntityTracker)
    BeginComponent(EntityModification)
        RegisterProperty(Entity, EntityModificationEntity)
    EndComponent()

    BeginComponent(EntityTracker)
        RegisterArrayProperty(EntityModification, EntityModifications)
        RegisterProperty(bool, EntityTrackerTrackValues)
    EndComponent()

    RegisterFunction(GetChanges)

    RegisterSubscription(PropertyChanged, OnPropertyChanged, 0)
    RegisterSubscription(EntityDestroyed, OnEntityDestroyed, 0)
EndUnit()