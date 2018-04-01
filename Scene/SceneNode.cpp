//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Core/Hierarchy.h>
#include "SceneNode.h"
#include "Scene.h"


    struct SceneNode {
        Entity Scene;
    };

    DefineComponent(SceneNode)
        Abstract()
    EndComponent()

    DefineEvent(SceneNodeSceneChanged, SceneNodeSceneChangedHandler)

    Entity GetSceneNodeScene(Entity sceneNode) {
        if(!HasSceneNode(sceneNode)) {
            return 0;
        }

        return GetSceneNode(sceneNode)->Scene;
    }

    Entity FindScene(Entity entity) {
        if(HasScene(entity)) {
            return entity;
        }

        if(HasHierarchy(entity) && IsEntityValid(GetParent(entity))) {
            return FindScene(GetParent(entity));
        }

        return 0;
    }

    void SetScene(Entity entity, Entity scene) {
        auto oldScene = GetSceneNodeScene(entity);
        auto newScene = scene;

        if(newScene != oldScene) {
            GetSceneNode(entity)->Scene = newScene;
            FireEvent(SceneNodeSceneChanged, entity, oldScene, newScene);

            for(auto child = GetFirstChild(entity); child; child = GetSibling(child)) {
                if(HasSceneNode(child)) {
                    SetScene(child, scene);
                }
            }
        }
    }

    static void OnParentChanged(Entity entity, Entity before, Entity after) {
        if(HasSceneNode(entity)) {
            auto scene = FindScene(entity);
            SetScene(entity, scene);
        }
    }

    static void OnSceneNodeAdded(Entity entity) {
        SetScene(entity, FindScene(entity));
    }

    static void OnSceneNodeRemoved(Entity entity) {
        SetScene(entity, 0);
    }

DefineService(SceneNode)
        Subscribe(ParentChanged, OnParentChanged)
        Subscribe(SceneNodeAdded, OnSceneNodeAdded)
        Subscribe(SceneNodeRemoved, OnSceneNodeRemoved)
EndService()
