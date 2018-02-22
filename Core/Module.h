//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_MODULE_H
#define PLAZA_MODULE_H

#include <Core/Handle.h>
#include <Core/Type.h>
#include <Core/Delegate.h>
#include "Service.h"


    DeclareHandle(Module)

    typedef void(*ModuleHandler)(Module module);

    DeclareEvent(ModuleInitialized, ModuleHandler)
    DeclareEvent(ModuleShutdown, ModuleHandler)

    void InitializeModule(Module module);
    void ShutdownModule(Module module);
    bool IsModuleInitialized(Module module);

    void SetModuleName(Module module, const char *name);
    const char * GetModuleName(Module module);

    void AddModuleService(Module module, Service service);
    void AddModuleType(Module module, Type type);
    void AddModuleDependency(Module module, Module dependency);

    Index GetModuleTypes(Module module);
    Type GetModuleType(Module module, Index index);

    Index GetModuleServices(Module module);
    Service GetModuleService(Module module, Index index);

    Index GetModuleDependencies(Module module);
    Module GetModuleDependency(Module module, Index index);

    void SetModuleSourcePath(Module module, StringRef sourceFilePath);
    StringRef GetModuleSourcePath(Module module);

#define DeclareModule(MODULE) \
    Module ModuleOf_ ## MODULE (); \

#define DefineModule(MODULE) \
    Module ModuleOf_ ## MODULE () { \
        static Module module = 0; \
        if(!module) { \
            module = CreateModule(); \
            SetModuleSourcePath(module, __FILE__); \
            SetModuleName(module, #MODULE);

#define ModuleService(SERVICE) \
            AddModuleService(module, ServiceOf_ ## SERVICE ());

#define ModuleType(TYPE) \
            AddModuleType(module, TypeOf_ ## TYPE ());

#define EndModule(MODULE) \
        } \
        return module;\
    }

#define ModuleDependency(DEPENDENCYMODULE) \
    AddModuleDependency(module, ModuleOf_ ## DEPENDENCYMODULE ());

#endif //PLAZA_MODULE_H
