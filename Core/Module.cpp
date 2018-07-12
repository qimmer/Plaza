//
// Created by Kim Johannsen on 07/01/2018.
//

#include <Core/Module.h>
#include <cstring>
#include "Entity.h"
#include "Debug.h"

typedef Entity(*ModuleOfSignature)();

struct Module {
    char ModuleVersion[128];
    char ModuleSourcePath[512];
};

BeginUnit(Module)
    BeginComponent(Module)
        RegisterProperty(StringRef, ModuleVersion)
        RegisterProperty(StringRef, ModuleSourcePath)
    EndComponent()

    RegisterFunction(GetModuleRoot)
    RegisterFunction(LoadModule)

    RegisterEvent(ModuleInitialized)
EndUnit()

API_EXPORT Entity GetModuleRoot() {
    static Entity root = CreateEntityFromName(0, "Modules");
    return root;
}

#ifdef WIN32
#include <Windows.h>
#include <dbghelp.h>

static Entity LoadModuleWin32(StringRef dllPath) {
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
        return 0;
    }

    pIDH=(PIMAGE_DOS_HEADER)hModule;

    if(pIDH->e_magic!=IMAGE_DOS_SIGNATURE)
    {
        Log(0, LogSeverity_Error, "File is not a DLL Library: %s", dllPath);
        return 0;
    }

    pINH=(PIMAGE_NT_HEADERS)((LPBYTE)hModule+pIDH->e_lfanew);

    if(pINH->Signature!=IMAGE_NT_SIGNATURE)
    {
        Log(0, LogSeverity_Error, "DLL Library has an invalid PE signature: %s", dllPath);
        return 0;
    }

    if(pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress==0)
    {
        Log(0, LogSeverity_Error, "DLL Library has no exports: %s", dllPath);
        return 0;
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
            return ((ModuleOfSignature)procAddress)();
        }
    }

    Log(0, LogSeverity_Error, "DLL Library contains no modules: %s", dllPath);

    return 0;
}

#endif

API_EXPORT Entity LoadModule(StringRef libraryPath) {
#ifdef WIN32
    return LoadModuleWin32(libraryPath);
#endif

    return false;
}
