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

    DefineService(SceneNode)
    EndService()

    DefineEvent(SceneNodeSceneChanged, SceneNodeSceneChangedHandler)

    Entity GetSceneNodeScene(Entity sceneNode) {
        if(!HasScene(sceneNode)) {
            return 0;
        }

        return GetSceneNode(sceneNode)->Scene;
    }

    static Entity FindScene(Entity entity) {
        if(HasScene(entity)) {
            return entity;
        }

        if(HasHierarchy(entity) && IsEntityValid(GetParent(entity))) {
            return FindScene(GetParent(entity));
        }

        return 0;
    }

    static void SetScene(Entity entity, Entity scene) {
        auto oldScene = GetSceneNodeScene(entity);
        auto newScene = scene;

        GetSceneNode(entity)->Scene = newScene;

        if(newScene != oldScene) {
            FireEvent(SceneNodeSceneChanged, entity, oldScene, newScene);

            for(auto child = GetFirstChild(entity); IsEntityValid(child); child = GetSibling(child)) {
                SetScene(child, scene);
            }
        }
    }

    static void OnParentChanged(Entity entity, Entity before, Entity after) {
        if(HasSceneNode(entity)) {
            SetScene(entity, FindScene(entity));
        }
    }

    static void OnSceneNodeAdded(Entity entity) {
        SetScene(entity, FindScene(entity));
    }

    static void OnSceneNodeRemoved(Entity entity) {
        SetScene(entity, 0);
    }

    static bool ServiceStart() {
        SubscribeParentChanged(OnParentChanged);
        SubscribeSceneNodeAdded(OnSceneNodeAdded);
        SubscribeSceneNodeAdded(OnSceneNodeRemoved);

        return true;
    }

    static bool ServiceStop() {
        UnsubscribeParentChanged(OnParentChanged);
        UnsubscribeSceneNodeAdded(OnSceneNodeAdded);
        UnsubscribeSceneNodeAdded(OnSceneNodeRemoved);

        return true;
    };
