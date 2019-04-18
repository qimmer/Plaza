//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_MODULE_H
#define PLAZA_MODULE_H

#include <Core/NativeUtils.h>
#include "Function.h"

struct ModuleRoot {
    ChildArray Modules, ModuleLoaders;
};

struct Dependency {
    Entity DependencyModule;
};

struct Module {
    StringRef ModuleVersion;
    StringRef ModuleSourcePath;
    StringRef ModuleBinaryPath;

    ChildArray Components, Functions, Systems, Enums, Extensions, Dependencies, Prefabs;
};

struct ModuleLoader {
    StringRef ModuleLoaderLibraryPath;
};

Unit(Module)

Component(ModuleLoader)
    Property(StringRef, ModuleLoaderLibraryPath)

Component(ModuleRoot)
    ArrayProperty(Module, Modules)
    ArrayProperty(ModuleLoader, ModuleLoaders)

Component(Dependency)
    ReferenceProperty(Module, DependencyModule)

Component(Module)
    Property(StringRef, ModuleSourcePath)
    Property(StringRef, ModuleBinaryPath)
    Property(StringRef, ModuleVersion)
    ArrayProperty(Component, Components)
    ArrayProperty(Function, Functions)
    ArrayProperty(System, Systems)
    ArrayProperty(Enum, Enums)
    ArrayProperty(Extension, Extensions)
    ArrayProperty(Dependency, Dependencies)
    ArrayProperty(Prefab, Prefabs)

Function(GetRoot, Entity)
Function(LoadPlazaModule, bool, StringRef libraryPath)

NativeFunctionInvoker1(bool, StringRef)

#endif //PLAZA_MODULE_H
