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
    DefinePropertyReactive(Entity, EntityPersistancePoint)
EndComponent()


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

DefineService(Persistance)
        Subscribe(EntityPersistancePointChanged, OnEntityPersistancePointChanged)
        Subscribe(ParentChanged, OnParentChanged)
EndService()
