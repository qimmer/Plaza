//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Core/Node.h>
#include "SceneNode.h"
#include "Scene.h"

struct SceneNode {
    Entity Scene;
};

BeginUnit(SceneNode)
    BeginComponent(SceneNode)
    Abstract()
EndComponent()

RegisterEvent(SceneNodeSceneChanged)

API_EXPORT Entity GetSceneNodeScene(Entity sceneNode) {
    if(!HasComponent(sceneNode, ComponentOf_SceneNode())) {
        return 0;
    }

    return GetSceneNode(sceneNode)->Scene;
}

API_EXPORT Entity FindScene(Entity entity) {
    if(HasComponent(entity, ComponentOf_Scene())) {
        return entity;
    }

    if(HasComponent(entity, ComponentOf_Node()) && IsEntityValid(GetParent(entity))) {
        return FindScene(GetParent(entity));
    }

    return 0;
}

API_EXPORT void SetScene(Entity entity, Entity scene) {
    if(HasComponent(entity, ComponentOf_SceneNode())) {
        auto oldScene = GetSceneNodeScene(entity);
        auto newScene = scene;

        if(newScene != oldScene) {
            GetSceneNodeData(entity)->Scene = newScene;
            FireEvent(EventOf_SceneNodeSceneChanged(), entity, oldScene, newScene);
        }
    }

    for(auto child = GetFirstChild(entity); child; child = GetSibling(child)) {
        SetScene(child, scene);
    }

}

LocalFunction(OnParentChanged, void, Entity entity, Entity before, Entity after) {
    if(HasComponent(entity, ComponentOf_SceneNode())) {
        auto scene = FindScene(entity);
        SetScene(entity, scene);
    }
}

LocalFunction(OnSceneNodeAdded, void, Entity component, Entity entity) {
    SetScene(entity, FindScene(entity));
}

LocalFunction(OnSceneNodeRemoved, void, Entity entity) {
    SetScene(entity, 0);
}

DefineService(SceneNode)
        RegisterSubscription(ParentChanged, OnParentChanged, 0)
        RegisterSubscription(SceneNodeAdded, OnSceneNodeAdded, 0)
        RegisterSubscription(SceneNodeRemoved, OnSceneNodeRemoved, 0)
EndService()
