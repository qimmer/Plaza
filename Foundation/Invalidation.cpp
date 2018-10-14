//
// Created by Kim on 22-08-2018.
//

#include "Invalidation.h"

struct Invalidation {
    bool DirtyFlag, Validating;
};

static eastl::vector<bool> entityDirtyFlags;

API_EXPORT bool IsDirty(Entity entity) {
    auto entityIndex = GetEntityIndex(entity);

    return entityDirtyFlags.size() > entityIndex && entityDirtyFlags[entityIndex];
}

API_EXPORT void Invalidate(Entity entity) {
    auto entityIndex = GetEntityIndex(entity);
    if(entityDirtyFlags.size() <= entityIndex) {
        entityDirtyFlags.resize(entityIndex + 1, false);
    }

    entityDirtyFlags[entityIndex] = true;
}

API_EXPORT void InvalidateParent(Entity child) {
    Invalidate(GetOwner(child));
}

API_EXPORT void Validate(Entity component) {
    Type types[] = { TypeOf_Entity };
    const void * argumentPtrs[] = { &component };

    FireEventFast(EventOf_Validate(), 1, types, argumentPtrs);

    Entity entity = 0;
    for(u32 i = GetNextComponent(component, InvalidIndex, NULL, &entity); i != InvalidIndex; i = GetNextComponent(component, i, NULL, &entity)) {
        entityDirtyFlags[GetEntityIndex(entity)] = false;
    }
}

BeginUnit(Invalidation)
    RegisterEvent(Validate)
EndUnit()
