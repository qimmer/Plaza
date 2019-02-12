//
// Created by Kim on 05-02-2019.
//

#ifndef PLAZA_MODULEPROFILING_H
#define PLAZA_MODULEPROFILING_H

#include <Core/NativeUtils.h>

struct ProfiledModule {
    Entity ProfiledModuleStartupStopWatch;
};

Unit(ModuleProfiling)
    Component(ProfiledModule)
        ChildProperty(StopWatch, ProfiledModuleStartupStopWatch)

#endif //PLAZA_MODULEPROFILING_H
