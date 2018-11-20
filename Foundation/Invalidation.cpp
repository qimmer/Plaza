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
    auto argument = MakeVariant(Entity, component);

    FireEventFast(EventOf_Validate(), 1, &argument);

    Entity entity = 0;

    for_entity_abstract(entity, data, component, {
        auto entityIndex = GetEntityIndex(entity);
        if(entityDirtyFlags.size() <= entityIndex) {
            entityDirtyFlags.resize(entityIndex + 1, false);
        }

        entityDirtyFlags[entityIndex] = false;
    });
}

BeginUnit(Invalidation)
    RegisterEvent(Validate)
EndUnit()
