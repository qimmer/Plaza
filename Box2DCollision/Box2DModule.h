//
// Created by Kim on 27-11-2018.
//

#ifndef PLAZA_BOX2DMODULE_H
#define PLAZA_BOX2DMODULE_H

#include <Core/Module.h>

Module(Box2DCollision)

Unit(Box2DCollision)
    Component(Box2DCollisionBody)
        Property(NativePtr, Body)
    Component(Box2DCollisionScene)
        Property(NativePtr, World)

    Declare(AppLoop, Box2D)
    Declare(StopWatch, Box2D)

#endif //PLAZA_BOX2DMODULE_H
