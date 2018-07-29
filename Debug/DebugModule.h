//
// Created by Kim on 27-07-2018.
//

#ifndef PLAZA_DEBUGMODULE_H
#define PLAZA_DEBUGMODULE_H

#include <Core/NativeUtils.h>

Module(Debug)

Unit(DebugServer)
    Component(Debug)
        ChildProperty(EntityTracker, EntityTracker)

#endif //PLAZA_DEBUGMODULE_H
