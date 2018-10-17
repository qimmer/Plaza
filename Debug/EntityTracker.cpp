//
// Created by Kim on 28-07-2018.
//

#include <Json/NativeUtils.h>
#include <Foundation/Stream.h>
#include <Core/Identification.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include "EntityTracker.h"
#include "DebugModule.h"

NativeFunctionInvoker2(u16, Entity, Entity)

typedef std::vector<bool> ChangeSet;

struct EntityModification {
    Entity EntityModificationEntity;
};

struct EntityTracker {
    ChangeSet *changedSet;
};

static bool debugInitialized = false;

static bool isAdding = false;

API_EXPORT u16 GetEntityTrackerChanges(Entity entityTracker, Entity responseStream) {
    SetStreamPath(responseStream, "memory://response.json");

    auto trackerData = GetEntityTrackerData(entityTracker);

    isAdding = true;
    for(auto i = 0; i < trackerData->changedSet->size(); ++i) {
        auto changed = (*trackerData->changedSet)[i];
        if(!changed) continue;

        (*trackerData->changedSet)[i] = false;
        auto entity = GetEntityByIndex(i);

        if(HasComponent(entity, ComponentOf_EntityModification())) continue;

        if(IsEntityValid(entity)) {
            SetEntityModificationEntity(AddEntityModifications(entityTracker), entity);
        }
    }

    auto responseCode = 200;
    if(!SerializeJson(
            responseStream,
            entityTracker,
            3, // Include changed entity's children, but not sub-children
            2)) { // Include entity tracker's modifications' entities
        responseCode = 500;
    }

    SetNumEntityModifications(entityTracker, 0);

    trackerData->changedSet->clear();

    isAdding = false;

    return responseCode;
}

static inline void TriggerChange(Entity entity) {
    if(isAdding || !IsEntityValid(entity)) return;

    if(HasComponent(entity, ComponentOf_EntityModification()) || HasComponent(entity, ComponentOf_EntityTracker())) {
        return;
    }

    auto index = GetEntityIndex(entity);
    for_entity(tracker, trackerData, EntityTracker) {

        if(index >= trackerData->changedSet->size()) {
            trackerData->changedSet->resize(index + 1, false);
        }

        (*trackerData->changedSet)[index] = true;
    }
}

LocalFunction(OnComponentAdded, void, Entity component, Entity entity) {
    if(component == ComponentOf_EntityModification()) return;

    if(component == ComponentOf_EntityTracker()) {
        GetEntityTrackerData(entity)->changedSet = new ChangeSet();
    } else {
        TriggerChange(entity);
    }
}

LocalFunction(OnComponentRemoved, void, Entity component, Entity entity) {
    if(component == ComponentOf_EntityTracker()) {
        delete GetEntityTrackerData(entity)->changedSet;
    } else {
        TriggerChange(entity);
    }
}

static void OnPropertyChanged(Entity property, Entity entity, Type valueType, const void *oldValue, const void *newValue) {
    TriggerChange(entity);
}

BeginUnit(EntityTracker)
    BeginComponent(EntityModification)
        RegisterProperty(Entity, EntityModificationEntity)
    EndComponent()

    BeginComponent(EntityTracker)
        RegisterArrayProperty(EntityModification, EntityModifications)
    EndComponent()

    RegisterFunctionSignature(NativeFunctionInvoker_u16_Entity_Entity, u16, Entity, Entity);

    RegisterFunction(GetEntityTrackerChanges)

    RegisterSubscription(EventOf_EntityComponentAdded(), OnComponentAdded, 0)
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnComponentRemoved, 0)
    //RegisterGenericPropertyChangedListener(OnPropertyChanged);
EndUnit()