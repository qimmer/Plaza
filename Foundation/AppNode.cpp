//
// Created by Kim Johannsen on 28/01/2018.
//

#include "AppNode.h"

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
    auto appNodeData = GetAppNode(entity);
    auto root = FindAppRoot(entity);

    if(appNodeData.AppNodeRoot != root) {
        SetAppNode(entity, {root});
    }

    for_entity(child, ComponentOf_AppNode()) {
        if(GetOwnership(child).Owner == entity) {
            SetAppNodeRootRecursive(child);
        }
    }
}

static void OnOwnerChanged(Entity entity, const Ownership& oldData, const Ownership& newData) {
    if(HasComponent(entity, ComponentOf_AppNode())) {
        SetAppNodeRootRecursive(entity);
    }
}

static void OnAppNodeChanged(Entity entity, const AppNode& oldData, const AppNode& newData) {
    if(!newData.AppNodeRoot) {
        SetAppNode(entity, {FindAppRoot(entity)});
    }
}

static void OnAppRootChanged(Entity entity, const AppRoot& oldData, const AppRoot& newData) {
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

    RegisterSystem(OnOwnerChanged, ComponentOf_Ownership())
    RegisterSystem(OnAppNodeChanged, ComponentOf_AppNode())
    RegisterSystem(OnAppRootChanged, ComponentOf_AppRoot())
EndUnit()
