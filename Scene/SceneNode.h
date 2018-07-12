//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_SCENENODE_H
#define PLAZA_SCENENODE_H

#include <Core/NativeUtils.h>


    Unit(SceneNode)
    Component(SceneNode)
    DeclareService(SceneNode)

    Entity GetSceneNodeScene(Entity sceneNode);
    Entity FindScene(Entity entity);
void SetScene(Entity entity, Entity scene);

    Event(SceneNodeSceneChanged, Entity oldScene, Entity newScene)

#endif //PLAZA_SCENENODE_H
