//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_SCENENODE_H
#define PLAZA_SCENENODE_H

#include <Core/Entity.h>


    DeclareComponent(SceneNode)
    DeclareService(SceneNode)

    typedef void(*SceneNodeSceneChangedHandler)(Entity sceneNode, Entity oldScene, Entity newScene);

    Entity GetSceneNodeScene(Entity sceneNode);

    DeclareEvent(SceneNodeSceneChanged, SceneNodeSceneChangedHandler)

#endif //PLAZA_SCENENODE_H
