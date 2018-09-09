//
// Created by Kim on 05-09-2018.
//

#include <Core/Debug.h>
#include "DllPersistance.h"
#include <Foundation/PersistancePoint.h>
#include <Foundation/NativeUtils.h>
#include "Stream.h"
//
// Created by Kim Johannsen on 20/02/2018.
//

static bool Serialize(Entity persistancePoint) {
    return false;
}

static bool Deserialize(Entity persistancePoint) {
    auto path = GetStreamPath(persistancePoint);

    if(memcmp(path, "file://", 7) != 0) {
        Log(0, LogSeverity_Error, "DLL library deserializer (loader) only supports file:// paths.");
        return false;
    }

    return LoadPlazaModule(path + 7);
}

LocalFunction(OnModuleBinaryPathChanged, void, Entity module, StringRef oldValue, StringRef newValue) {
    if(strlen(newValue) > 0) {
        char path[512];
        snprintf(path, 512, "file://%s", newValue);
        SetStreamPath(module, path);
    }
}

BeginUnit(DllPersistance)
    RegisterSerializer(DllSerializer, "application/dll")

    auto fileType = AddModuleFileTypes(module);
    SetFileTypeComponent(fileType, ComponentOf_Module());
    SetFileTypeExtension(fileType, ".dll");
    SetFileTypeMimeType(fileType, "application/dll");

    fileType = AddModuleFileTypes(module);
    SetFileTypeComponent(fileType, ComponentOf_Module());
    SetFileTypeExtension(fileType, ".dylib");
    SetFileTypeMimeType(fileType, "application/dll");

    fileType = AddModuleFileTypes(module);
    SetFileTypeComponent(fileType, ComponentOf_Module());
    SetFileTypeExtension(fileType, ".so");
    SetFileTypeMimeType(fileType, "application/dll");

    RegisterSubscription(ModuleBinaryPathChanged, OnModuleBinaryPathChanged, 0)
EndUnit()
