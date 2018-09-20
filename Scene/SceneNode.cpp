//
// Created by Kim Johannsen on 28/01/2018.
//

#include "SceneNode.h"
#include "Scene.h"

struct SceneNode {
    Entity SceneNodeScene;
};

static Entity FindScene(Entity entity) {
    if(!IsEntityValid(entity)) {
        return 0;
    }

    if(HasComponent(entity, ComponentOf_Scene())) {
        return entity;
    }

    return FindScene(GetOwner(entity));
}

LocalFunction(OnOwnerChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_SceneNode())) {
        SetSceneNodeScene(entity, FindScene(entity));
    }
}

LocalFunction(OnSceneNodeAdded, void, Entity component, Entity entity) {
    SetSceneNodeScene(entity, FindScene(entity));
}

BeginUnit(SceneNode)
    BeginComponent(SceneNode)
        RegisterReferencePropertyReadOnly(Scene, SceneNodeScene)
    EndComponent()

    RegisterSubscription(OwnerChanged, OnOwnerChanged, 0)
    RegisterSubscription(EntityComponentAdded, OnSceneNodeAdded, ComponentOf_SceneNode())
EndUnit()
