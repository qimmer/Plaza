//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Scene.h"
#include <Foundation/AppNode.h>

struct Scene {
};

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

    return FindScene(GetOwnership(entity).Owner);
}

static void SetSceneNodeSceneRecursive(Entity entity) {
    SetSceneNodeScene(entity, FindScene(entity));
    for_entity(child, ComponentOf_SceneNode()) {
        if(GetOwnership(child).Owner == entity) {
            SetSceneNodeSceneRecursive(child);
        }
    }
}

LocalFunction(OnOwnerChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_SceneNode())) {
        SetSceneNodeSceneRecursive(entity);
    }
}

LocalFunction(OnSceneNodeAdded, void, Entity component, Entity entity) {
    SetSceneNodeScene(entity, FindScene(entity));
}

LocalFunction(OnSceneAdded, void, Entity component, Entity entity) {
    SetSceneNodeSceneRecursive(entity);
}

BeginUnit(Scene)
    BeginComponent(SceneNode)
        RegisterBase(AppNode)
        RegisterReferencePropertyReadOnly(Scene, SceneNodeScene)
    EndComponent()
    BeginComponent(Scene)
        RegisterBase(SceneNode)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
    RegisterSubscription(EventOf_EntityComponentAdded(), OnSceneNodeAdded, ComponentOf_SceneNode())
    RegisterSubscription(EventOf_EntityComponentAdded(), OnSceneAdded, ComponentOf_Scene())
EndUnit()
