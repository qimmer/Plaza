//
// Created by Kim on 29-09-2018.
//

#ifndef PLAZA_SCENE_H
#define PLAZA_SCENE_H

#include <Core/NativeUtils.h>

struct Scene {
};

struct SceneNode {
    Entity SceneNodeScene;
};

Unit(Scene)
    Component(Scene)
    Component(SceneNode)
        ReferenceProperty(Scene, SceneNodeScene)

#endif //PLAZA_SCENE_H
