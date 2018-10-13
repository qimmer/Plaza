//
// Created by Kim Johannsen on 28/01/2018.
//

#include "AppNode.h"

struct AppRoot {
    bool AppRootActive;
};

struct AppNode {
    Entity AppNodeRoot;
    Vector(AppNodeChildren, Entity, 32);
};

static Entity FindAppRoot(Entity entity) {
    if(!IsEntityValid(entity)) {
        return 0;
    }

    if(HasComponent(entity, ComponentOf_AppRoot())) {
        return entity;
    }

    return FindAppRoot(GetOwner(entity));
}

LocalFunction(OnOwnerChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_AppNode())) {
        SetAppNodeRoot(entity, FindAppRoot(entity));
    }
}

LocalFunction(OnAppNodeAdded, void, Entity component, Entity entity) {
    SetAppNodeRoot(entity, FindAppRoot(entity));
}

BeginUnit(AppNode)
    BeginComponent(AppNode)
        RegisterReferencePropertyReadOnly(AppRoot, AppNodeRoot)
        RegisterArrayProperty(AppNode, AppNodeChildren)
    EndComponent()
    BeginComponent(AppRoot)
        RegisterBase(AppNode)
        RegisterProperty(bool, AppRootActive)
    EndComponent()

    RegisterSubscription(OwnerChanged, OnOwnerChanged, 0)
    RegisterSubscription(EntityComponentAdded, OnAppNodeAdded, ComponentOf_AppNode())
EndUnit()
