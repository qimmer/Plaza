//
// Created by Kim on 28-07-2018.
//

#include "EntityTracker.h"

struct EntityModification {
    Entity EntityModificationEntity;
};

struct EntityTracker {
    Vector(EntityModifications, Entity, 128);
};

LocalFunction(OnPropertyChanged, void, Entity property, Entity entity) {
    if(property == PropertyOf_EntityModificationEntity() || property == PropertyOf_EntityModifications()) return;

    if(GetPropertyKind(property) == PropertyKind_Array) {
        for_entity(tracker, trackerData, EntityTracker) {
            bool found = false;
            for(auto i = 0; i < trackerData->EntityModifications.Count; ++i) {
                auto modification = GetVector(trackerData->EntityModifications)[i];
                auto modificationData = GetEntityModificationData(modification);

                if(modificationData->EntityModificationEntity == entity) {
                    found = true;
                    break;
                }
            }

            if(!found) {
                auto modification = AddEntityModifications(tracker);
                SetEntityModificationEntity(modification, entity);
            }
        }
    }
}

BeginUnit(EntityTracker)
    BeginComponent(EntityModification)
        RegisterProperty(Entity, EntityModificationEntity)
    EndComponent()

    BeginComponent(EntityTracker)
        RegisterArrayProperty(EntityModification, EntityModifications)
    EndComponent()

    RegisterSubscription(PropertyChanged, OnPropertyChanged, 0)
EndUnit()