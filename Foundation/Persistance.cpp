//
// Created by Kim Johannsen on 25/01/2018.
//

#include "Persistance.h"
#include <Core/Hierarchy.h>


struct Persistance {
    Entity EntityPersistancePoint;
};

DefineComponent(Persistance)
    Dependency(Hierarchy)
    DefineProperty(Entity, EntityPersistancePoint)
EndComponent()

DefineService(Persistance)
EndService()

DefineComponentPropertyReactive(Persistance, Entity, EntityPersistancePoint)

static void OnEntityPersistancePointChanged(Entity entity, Entity oldStream, Entity newStream) {
    for(auto child = GetFirstChild(entity); IsEntityValid(child); child = GetSibling(child)) {
        SetEntityPersistancePoint(child, newStream);
    }
}

static void OnParentChanged(Entity entity, Entity oldParent, Entity newParent) {
    if(HasPersistance(oldParent) && HasPersistance(entity)) {
        RemovePersistance(entity);
    }

    if(HasPersistance(newParent)) {
        SetEntityPersistancePoint(entity, GetEntityPersistancePoint(newParent));
    }
}

static bool ServiceStart() {
    SubscribeEntityPersistancePointChanged(OnEntityPersistancePointChanged);
    SubscribeParentChanged(OnParentChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeEntityPersistancePointChanged(OnEntityPersistancePointChanged);
    UnsubscribeParentChanged(OnParentChanged);
    return false;
}
