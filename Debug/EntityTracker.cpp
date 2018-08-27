//
// Created by Kim on 28-07-2018.
//

#include <Json/NativeUtils.h>
#include <Foundation/Stream.h>
#include <Core/Identification.h>
#include <Networking/Replication.h>
#include <Networking/NetworkingModule.h>
#include <Foundation/AppLoop.h>
#include "EntityTracker.h"
#include "DebugModule.h"

struct EntityModification {
    Entity EntityModificationEntity;
};

struct EntityTracker {
    Vector(EntityModifications, Entity, 128);
    u64 LastFrame;
};

API_EXPORT u16 GetChanges(Entity responseStream, StringRef path) {
    SetStreamPath(responseStream, "memory://response.json");

    auto entityTracker = GetEntityTracker(ModuleOf_Debug());
    auto trackerData = GetEntityTrackerData(entityTracker);

    for_entity(entity, data, Replication) {
        if(*(u64*)data >= trackerData->LastFrame) {
            SetEntityModificationEntity(AddEntityModifications(entityTracker), entity);
        }
    }

    trackerData->LastFrame = GetAppLoopFrame(GetReplicationAppLoop(ModuleOf_Networking()));

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
    EndComponent()

    RegisterFunction(GetChanges)
EndUnit()