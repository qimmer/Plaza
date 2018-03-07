//
// Created by Kim Johannsen on 16/01/2018.
//

#include "Invalidation.h"

struct Invalidation {
    bool Invalidated;
};

static std::unordered_set<Entity> invalidatedEntities;

DefineComponent(Invalidation)
EndComponent()

DefineService(Invalidation)
EndService()

DefineComponentProperty(Invalidation, bool, Invalidated)

void ValidateAll(EntityHandler validator, EntityBoolHandler condition) {
    static std::unordered_set<Entity> copies;
    copies = invalidatedEntities;

    for(auto entity : copies) {
        if(!IsEntityValid(entity) || !HasInvalidation(entity)) {
            invalidatedEntities.erase(entity);
        } else {
            if(condition(entity)) {
                SetInvalidated(entity, false);
                validator(entity);
            }
        }
    }
}

static void OnComponentChanged(Entity entity, Type type) {
    if(type != TypeOf_Invalidation() && HasInvalidation(entity)) {
        SetInvalidated(entity, true);
    }
}

static void OnInvalidatedChanged(Entity entity, bool oldValue, bool newValue) {
    if(!oldValue && newValue) {
        invalidatedEntities.insert(entity);
    }

    if(oldValue && !newValue) {
        invalidatedEntities.erase(entity);
    }
}

static bool ServiceStart() {
    SubscribeComponentChanged(OnComponentChanged);
    SubscribeInvalidatedChanged(OnInvalidatedChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeComponentChanged(OnComponentChanged);
    UnsubscribeInvalidatedChanged(OnInvalidatedChanged);
    return true;
}
