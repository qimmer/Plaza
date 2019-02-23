//
// Created by Kim Johannsen on 28/01/2018.
//

#include "AppNode.h"

struct AppRoot {
    bool AppRootActive;
};

struct AppNode {
    Entity AppNodeRoot;
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

static void SetAppNodeRootRecursive(Entity entity) {
    SetAppNodeRoot(entity, FindAppRoot(entity));
    for_entity(child, data, AppNode) {
        if(GetOwner(child) == entity) {
            SetAppNodeRootRecursive(child);
        }
    }
}

LocalFunction(OnOwnerChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_AppNode())) {
        SetAppNodeRootRecursive(entity);
    }
}

LocalFunction(OnAppNodeAdded, void, Entity component, Entity entity) {
    SetAppNodeRoot(entity, FindAppRoot(entity));
}

LocalFunction(OnAppRootAdded, void, Entity component, Entity entity) {
    SetAppNodeRootRecursive(entity);
}

BeginUnit(AppNode)
    BeginComponent(AppNode)
        RegisterReferencePropertyReadOnly(AppRoot, AppNodeRoot)
        RegisterArrayProperty(AppNode, Children)
    EndComponent()
    BeginComponent(AppRoot)
        RegisterBase(AppNode)
        RegisterProperty(bool, AppRootActive)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
    RegisterSubscription(EventOf_EntityComponentAdded(), OnAppNodeAdded, ComponentOf_AppNode())
    RegisterSubscription(EventOf_EntityComponentAdded(), OnAppRootAdded, ComponentOf_AppRoot())
EndUnit()
