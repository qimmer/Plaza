//
// Created by Kim on 22-08-2018.
//

#include "Invalidation.h"

struct Invalidation {
    bool DirtyFlag;
};

API_EXPORT void Invalidate(Entity entity) {
    if(HasComponent(entity, ComponentOf_Invalidation())) {
        SetDirtyFlag(entity, true);
    }
}

API_EXPORT void Validate(Entity entity) {
    if(HasComponent(entity, ComponentOf_Invalidation())) {
        SetDirtyFlag(entity, false);

        Type types[] = { TypeOf_Entity };
        const void * argumentPtrs[] = { &entity };

        FireEventFast(EventOf_Validate(), 1, types, argumentPtrs);
    }
}

LocalFunction(OnInvalidationAdded, void, Entity entity) {
    GetInvalidationData(entity)->DirtyFlag = true;
}

BeginUnit(Invalidation)
    BeginComponent(Invalidation)
        RegisterProperty(bool, DirtyFlag)
    EndComponent()

    RegisterEvent(Validate)

    RegisterSubscription(EntityComponentAdded, OnInvalidationAdded, ComponentOf_Invalidation())
EndUnit()
