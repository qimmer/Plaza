//
// Created by Kim Johannsen on 16/01/2018.
//

#include "Invalidation.h"
#include "AppLoop.h"


    struct Invalidation {
        bool Invalidated;
    };

    DefineComponent(Invalidation)
    EndComponent()

    DefineService(Invalidation)
    EndService()

    DefineComponentProperty(Invalidation, bool, Invalidated)

    void ValidateAll(EntityHandler validator, EntityBoolHandler condition) {
        for_entity(entity, HasInvalidation) {
            if(GetInvalidated(entity) && condition(entity)) {
                SetInvalidated(entity, false);
                validator(entity);
            }
        }
    }

    static void OnComponentChanged(Entity entity, Type type) {
        if(type != TypeOf_Invalidation()) {
            SetInvalidated(entity, true);
        }
    }

    static bool ServiceStart() {
        SubscribeComponentChanged(OnComponentChanged);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeComponentChanged(OnComponentChanged);
        return true;
    }
