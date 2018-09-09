//
// Created by Kim Johannsen on 24/01/2018.
//

#ifndef PLAZA_COREMODULE_H
#define PLAZA_COREMODULE_H

#include <Core/Module.h>

Module(Core)
    Unit(Core)
        Component(CoreModule)
            ArrayProperty(LogMessage, LogMessages)

#endif //PLAZA_COREMODULE_H
