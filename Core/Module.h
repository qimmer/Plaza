//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_MODULE_H
#define PLAZA_MODULE_H

#include <Core/NativeUtils.h>
#include <Core/Event.h>
#include "Function.h"

Unit(Module)

Component(ModuleRoot)
    ArrayProperty(Module, Modules)

Component(Module)
    Property(StringRef, ModuleSourcePath)
    Property(StringRef, ModuleBinaryPath)
    Property(StringRef, ModuleVersion)
    ArrayProperty(Component, Components)
    ArrayProperty(Function, Functions)
    ArrayProperty(Event, Events)
    ArrayProperty(Enum, Enums)
    ArrayProperty(Extension, Extensions)
    ArrayProperty(Subscription, Subscriptions)
    ArrayProperty(Identification, Roots)

Event(ModuleInitialized)

Function(GetModuleRoot, Entity)
Function(LoadPlazaModule, bool, StringRef libraryPath)

void __InitializeModule();

#endif //PLAZA_MODULE_H
