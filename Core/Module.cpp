//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Module.h>
#include <cstring>
#include "Entity.h"
#include "Enum.h"
#include "Debug.h"
#include "Vector.h"
#include "Identification.h"

typedef Entity(*ModuleOfSignature)();

struct ModuleRoot {
    Vector(Modules, Entity, 64)
    Vector(ModuleLoaders, Entity, 64)
};

struct Dependency {
    Entity DependencyModule;
};

struct Module {
    StringRef ModuleVersion;
    StringRef ModuleSourcePath;
    StringRef ModuleBinaryPath;

    Vector(Dependencies, Entity, 16)
    Vector(Components, Entity, 64)
    Vector(Events, Entity, 64)
    Vector(Enums, Entity, 16)
    Vector(Functions, Entity, 128)
    Vector(Extensions, Entity, 64)
    Vector(Subscriptions, Entity, 128)
    Vector(Roots, Entity, 128)
};

struct ModuleLoader {
    StringRef ModuleLoaderLibraryPath;
};

LocalFunction(OnModuleLoaderLibraryPathChanged, void, Entity loader, StringRef oldPath, StringRef newPath) {
    LoadPlazaModule(newPath);
}

API_EXPORT Entity GetModuleRoot() {
    static Entity root = 0;
    if(root == 0) {
        root = __CreateEntity();
        AddComponent(root, ComponentOf_ModuleRoot());
        SetName(root, "ModuleRoot");
        SetUuid(root, "00000000-0000-0000-0000-000000000000");
    }
    return root;
}

#ifdef WIN32
#undef Enum
#include <Windows.h>
#include <dbghelp.h>

static Entity LoadModuleWin32(StringRef dllPath) {
    bool found = false;
    PIMAGE_DOS_HEADER pIDH;
    PIMAGE_NT_HEADERS pINH;
    PIMAGE_EXPORT_DIRECTORY pIED;

    HMODULE hModule;
    PDWORD Address,Name;
    PWORD Ordinal;

    DWORD i;

    hModule=LoadLibrary(dllPath);

    if(!hModule)
    {
        Log(0, LogSeverity_Error, "DLL Library Not Found: %s", dllPath);
        return false;
    }

    pIDH=(PIMAGE_DOS_HEADER)hModule;

    if(pIDH->e_magic!=IMAGE_DOS_SIGNATURE)
    {
        Log(0, LogSeverity_Error, "File is not a DLL Library: %s", dllPath);
        return false;
    }

    pINH=(PIMAGE_NT_HEADERS)((LPBYTE)hModule+pIDH->e_lfanew);

    if(pINH->Signature!=IMAGE_NT_SIGNATURE)
    {
        Log(0, LogSeverity_Error, "DLL Library has an invalid PE signature: %s", dllPath);
        return false;
    }

    if(pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress==0)
    {
        Log(0, LogSeverity_Error, "DLL Library has no exports: %s", dllPath);
        return false;
    }

    pIED=(PIMAGE_EXPORT_DIRECTORY)((LPBYTE)hModule+pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    Address=(PDWORD)((LPBYTE)hModule+pIED->AddressOfFunctions);
    Name=(PDWORD)((LPBYTE)hModule+pIED->AddressOfNames);

    Ordinal=(PWORD)((LPBYTE)hModule+pIED->AddressOfNameOrdinals);

    for(i=0;i<pIED->NumberOfNames;i++)
    {
        auto name = (char*)hModule+Name[i];
        if(memcmp(name + 4, "ModuleOf_", 9) == 0) {
            auto procAddress = (PVOID)((LPBYTE)hModule+Address[Ordinal[i]]);
            auto module = ((ModuleOfSignature)procAddress)();

            SetModuleBinaryPath(module, dllPath);
            found = true;
        }
    }

    if(!found) {
        Log(0, LogSeverity_Error, "DLL Library contains no modules: %s", dllPath);
    }

    return found;
}
#else

#include <Core/Unix/ELFModule.h>

static bool LoadModuleUnix(StringRef libraryPath) {
    return LoadElfModule(libraryPath);
}
#endif

API_EXPORT bool LoadPlazaModule(StringRef libraryPath) {
#ifdef WIN32
    return LoadModuleWin32(libraryPath);
#else
    return LoadModuleUnix(libraryPath);
#endif
}


void __InitializeModule() {
    auto component = ComponentOf_Module();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Module));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Components(), offsetof(Module, Components), sizeof(Module::Components), TypeOf_Entity, component, ComponentOf_Component(), PropertyKind_Array);
    __Property(PropertyOf_Events(), offsetof(Module, Events), sizeof(Module::Events), TypeOf_Entity, component, ComponentOf_Event(), PropertyKind_Array);
    __Property(PropertyOf_Functions(), offsetof(Module, Functions), sizeof(Module::Functions), TypeOf_Entity, component, ComponentOf_Function(), PropertyKind_Array);
    __Property(PropertyOf_Extensions(), offsetof(Module, Extensions), sizeof(Module::Extensions), TypeOf_Entity, component, ComponentOf_Extension(), PropertyKind_Array);
    __Property(PropertyOf_Subscriptions(), offsetof(Module, Subscriptions), sizeof(Module::Subscriptions), TypeOf_Entity, component, ComponentOf_Subscription(), PropertyKind_Array);
    __Property(PropertyOf_Dependencies(), offsetof(Module, Dependencies), sizeof(Module::Dependencies), TypeOf_Entity, component, ComponentOf_Module(), PropertyKind_Array);

    component = ComponentOf_ModuleRoot();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(ModuleRoot));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_Modules(), offsetof(ModuleRoot, Modules), sizeof(ModuleRoot::Modules), TypeOf_Entity, component, ComponentOf_Module(), PropertyKind_Array);
}


BeginUnit(Module)
    BeginComponent(ModuleLoader)
        RegisterProperty(StringRef, ModuleLoaderLibraryPath)
    EndComponent()

    BeginComponent(ModuleRoot)
        RegisterArrayProperty(Module, Modules)
        RegisterArrayProperty(ModuleLoader, ModuleLoaders)
    EndComponent()

    BeginComponent(Dependency)
        RegisterReferenceProperty(Module, DependencyModule)
    EndComponent()

    BeginComponent(Module)
        RegisterProperty(StringRef, ModuleVersion)
        RegisterProperty(StringRef, ModuleSourcePath)
        RegisterProperty(StringRef, ModuleBinaryPath)

        RegisterArrayProperty(Component, Components)
        RegisterArrayProperty(Event, Events)
        RegisterArrayProperty(Enum, Enums)
        RegisterArrayProperty(Function, Functions)
        RegisterArrayProperty(Extension, Extensions)
        RegisterArrayProperty(Subscription, Subscriptions)
        RegisterArrayProperty(Dependency, Dependencies)
    EndComponent()

    RegisterFunction(GetModuleRoot)
    RegisterFunction(LoadPlazaModule)

    RegisterEvent(ModuleInitialized)

    RegisterSubscription(ModuleLoaderLibraryPathChanged, OnModuleLoaderLibraryPathChanged, 0)
EndUnit()
