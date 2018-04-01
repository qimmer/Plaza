//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Module.h>
#include <Core/Pool.h>
#include <Core/String.h>
#include <algorithm>
#include "Service.h"

struct ModuleData {
    bool initialized;
    String name, sourceFilePath;
    Vector<Type> types;
    Vector<Service> services;
    Vector<Module> dependencies;
    Handler initializeFunc, shutdownFunc;
};

DefineHandle(Module, ModuleData)

DefineEvent(ModuleInitialized, ModuleHandler)
DefineEvent(ModuleShutdown, ModuleHandler)

void InitializeModule(Module module) {
    auto data = ModuleAt(module);

    if(!IsModuleInitialized(module)) {
        for(auto dependency : data->dependencies) {
            InitializeModule(dependency);
        }

        Log(LogChannel_Core, LogSeverity_Info, "Registering Module '%s' ...", GetModuleName(module));

        for(auto service : data->services) {
            StartService(service);
        }
        data->initialized = true;
    }
}

void ShutdownModule(Module module) {
    auto data = ModuleAt(module);

    if(IsModuleInitialized(module)) {
        Log(LogChannel_Core, LogSeverity_Info, "Unregistering Module '%s' ...", GetModuleName(module));

        for(auto dependee = GetNextModule(0); IsModuleValid(dependee); dependee = GetNextModule(dependee)) {
            auto& dependencies = ModuleAt(dependee)->dependencies;
            if(std::find(dependencies.begin(), dependencies.end(), module) != dependencies.end()) {
                ShutdownModule(dependee);
            }
        }

        for(auto service : data->services) {
            StopService(service);
        }
        ModuleAt(module)->initialized = false;
    }
}

bool IsModuleInitialized(Module module) {
    return ModuleAt(module)->initialized;
}

void SetModuleFunctions(Module module, Handler initializeFunc, Handler shutdownFunc) {
    auto data = ModuleAt(module);
    ModuleAt(module)->initializeFunc = initializeFunc;
    ModuleAt(module)->shutdownFunc = shutdownFunc;
}

void SetModuleName(Module module, const char *name) {
    ModuleAt(module)->name = name;
}

const char *GetModuleName(Module module) {
    return ModuleAt(module)->name.c_str();
}

void AddModuleService(Module module, Service service) {
    ModuleAt(module)->services.push_back(service);
}

void AddModuleType(Module module, Type type) {
    ModuleAt(module)->types.push_back(type);
}

void AddModuleDependency(Module module, Module dependency) {
    ModuleAt(module)->dependencies.push_back(dependency);
}

Index GetModuleTypes(Module module) {
    return ModuleAt(module)->types.size();
}

Type GetModuleType(Module module, Index index) {
    return ModuleAt(module)->types[index];
}

Index GetModuleServices(Module module) {
    return ModuleAt(module)->services.size();
}

Service GetModuleService(Module module, Index index) {
    return ModuleAt(module)->services[index];
}

Index GetModuleDependencies(Module module) {
    return ModuleAt(module)->dependencies.size();
}

Module GetModuleDependency(Module module, Index index) {
    return ModuleAt(module)->dependencies[index];
}

void SetModuleSourcePath(Module module, StringRef sourceFilePath) {
    ModuleAt(module)->sourceFilePath = sourceFilePath;
}

StringRef GetModuleSourcePath(Module module) {
    return ModuleAt(module)->sourceFilePath.c_str();
}
