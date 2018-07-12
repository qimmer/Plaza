//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_MODULE_H
#define PLAZA_MODULE_H

#include <Core/NativeUtils.h>
#include <Core/Event.h>
#include "Function.h"

Unit(Module)

Component(Module)
    Property(StringRef, ModuleSourcePath)
    Property(StringRef, ModuleBinaryPath)
    Property(StringRef, ModuleVersion)

Event(ModuleInitialized)

Function(GetModuleRoot, Entity)
Function(LoadModule, Entity, StringRef libraryPath)

#endif //PLAZA_MODULE_H
