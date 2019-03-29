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

    return FindAppRoot(GetOwnership(entity).Owner);
}

static void SetAppNodeRootRecursive(Entity entity) {
    SetAppNode(entity, {FindAppRoot(entity)});
    for_entity(child, ComponentOf_AppNode()) {
        if(GetOwnership(child).Owner == entity) {
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
    SetAppNode(entity, {FindAppRoot(entity)});
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
