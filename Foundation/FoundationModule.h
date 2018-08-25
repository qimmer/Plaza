//
// Created by Kim Johannsen on 08/01/2018.
//

#ifndef PLAZA_FOUNDATIONMODULE_H
#define PLAZA_FOUNDATIONMODULE_H

#include <Core/Module.h>

Module(Foundation)
    Unit(Foundation)
        Component(FoundationData)
            ChildProperty(TaskQueue, TaskQueue)
		Component(FoundationModule)
			ArrayProperty(FileType, FileTypes)

#endif //PLAZA_FOUNDATIONMODULE_H
