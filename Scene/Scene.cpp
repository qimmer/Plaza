//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Scene.h"
#include <Foundation/AppNode.h>

static Entity FindScene(Entity entity) {
    if(!IsEntityValid(entity)) {
        return 0;
    }

    if(HasComponent(entity, ComponentOf_Scene())) {
        return entity;
    }

    return FindScene(GetOwnership(entity).Owner);
}

static void SetSceneNodeSceneRecursive(Entity entity) {
    SetSceneNode(entity, {FindScene(entity)});
    for_entity(child, ComponentOf_SceneNode()) {
        if(GetOwnership(child).Owner == entity) {
            SetSceneNodeSceneRecursive(child);
        }
    }
}

static void OnOwnershipChanged(Entity entity, const Ownership& oldData, const Ownership& newData) {
    if(HasComponent(entity, ComponentOf_SceneNode())) {
        SetSceneNodeSceneRecursive(entity);
    }
}

static void OnSceneNodeChanged(Entity entity, const SceneNode& oldData, const SceneNode& newData) {
    if(!oldData.SceneNodeScene && !newData.SceneNodeScene) {
        SetSceneNode(entity, {FindScene(entity)});
    }
}

BeginUnit(Scene)
    BeginComponent(SceneNode)
        RegisterBase(AppNode)
        RegisterReferencePropertyReadOnly(Scene, SceneNodeScene)
    EndComponent()
    BeginComponent(Scene)
        RegisterBase(SceneNode)
    EndComponent()

    RegisterSystem(OnOwnershipChanged, ComponentOf_Ownership())
    RegisterSystem(OnSceneNodeChanged, ComponentOf_SceneNode())
EndUnit()
