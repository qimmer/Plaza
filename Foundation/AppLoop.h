//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_APPLOOP_H
#define PLAZA_APPLOOP_H

#include <Core/Event.h>

Unit(AppLoop)
    Component(AppLoop)
        Property(u64, AppLoopFrame)

    Function(RunAppLoop, void, Entity appLoop)
    Function(RunMainAppLoop, void)


#endif //PLAZA_APPLOOP_H
