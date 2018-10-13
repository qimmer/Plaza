//
// Created by Kim on 22-08-2018.
//

#include "Invalidation.h"

struct Invalidation {
    bool DirtyFlag, Validating;
};

API_EXPORT void Invalidate(Entity entity) {
    auto data = GetInvalidationData(entity);
    if(data && data->Validating) return;

    SetDirtyFlag(entity, true);
}

API_EXPORT void Validate(Entity entity) {
    auto data = GetInvalidationData(entity);
    if(data->DirtyFlag) {
        SetDirtyFlag(entity, false);

        data->Validating = true;
        Type types[] = { TypeOf_Entity };
        const void * argumentPtrs[] = { &entity };

        FireEventFast(EventOf_Validate(), 1, types, argumentPtrs);

        data->Validating = false;
    }
}

LocalFunction(OnInvalidationAdded, void, Entity component, Entity entity) {
    GetInvalidationData(entity)->DirtyFlag = true;
}

LocalFunction(OnPropertyChanged, void, Entity property, Entity entity) {
    if(property == PropertyOf_DirtyFlag()) return;

    auto data = GetInvalidationData(entity);
    if(data && !data->DirtyFlag) {
        Invalidate(entity);
    }
}

BeginUnit(Invalidation)
    BeginComponent(Invalidation)
        RegisterProperty(bool, DirtyFlag)
    EndComponent()

    RegisterEvent(Validate)

    RegisterSubscription(EntityComponentAdded, OnInvalidationAdded, ComponentOf_Invalidation())
    RegisterSubscription(PropertyChanged, OnPropertyChanged, 0)
EndUnit()
