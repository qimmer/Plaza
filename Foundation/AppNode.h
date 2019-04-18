//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_APPNODE_H
#define PLAZA_APPNODE_H

#include <Core/NativeUtils.h>

struct AppRoot {
    bool AppRootActive;
};

struct AppNode {
    Entity AppNodeRoot;
    ChildArray Children;
};

Unit(AppNode)
    Component(AppRoot)
        Property(bool, AppRootActive)

    Component(AppNode)
        ReferenceProperty(AppRoot, AppNodeRoot)
        ArrayProperty(AppNode, Children)

#endif //PLAZA_SCENE_H
