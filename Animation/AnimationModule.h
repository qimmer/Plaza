//
// Created by Kim on 11-10-2018.
//

#ifndef PLAZA_ANIMATIONMODULE_H
#define PLAZA_ANIMATIONMODULE_H

#include <Core/Module.h>

Module(Animation)
    Unit(AnimationModule)
    Component(AnimationModule)
        ChildProperty(StopWatch, AnimationStopWatch)

#endif //PLAZA_ANIMATIONMODULE_H
