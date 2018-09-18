//
// Created by Kim Johannsen on 08/01/2018.
//

#include <Core/NativeUtils.h>

#include <Foundation/AppLoop.h>
#include <Foundation/CommandLine.h>
#include <Foundation/Invocation.h>
#include <Foundation/Stream.h>
#include <Foundation/MemoryStream.h>
#include <Foundation/PersistancePoint.h>
#include <Foundation/StopWatch.h>
#include <Foundation/Task.h>
#include <Foundation/Timer.h>
#include <Foundation/ValueConverter.h>
#include <Foundation/VirtualPath.h>
#include <Foundation/Visibility.h>
#include <Foundation/Invalidation.h>
#include <Foundation/SerializationSettings.h>
#include "FoundationModule.h"

struct FoundationModule {
    Entity TaskQueue, FilesystemUpdateTimer, PersistancePointSerializationSettings;
};

struct FoundationExtension {
	Vector(FileTypes, Entity, 32)
};

BeginUnit(Foundation)
    BeginComponent(FoundationModule)
        RegisterChildProperty(TaskQueue, TaskQueue)
        RegisterChildProperty(SerializationSettings, PersistancePointSerializationSettings)
    EndComponent()
	BeginComponent(FoundationExtension)
		RegisterArrayProperty(FileType, FileTypes)
	EndComponent()

	AddComponent(module, ComponentOf_FoundationModule());

	auto settings = GetPersistancePointSerializationSettings(ModuleOf_Foundation());
	SetSerializationSettingsExcludeNativeEntities(settings, true);
    SetSerializationSettingsExcludePersistedChildren(settings, true);
    SetSerializationSettingsMaxChildLevel(settings, UINT32_MAX);
EndUnit()

BeginModule(Foundation)
    RegisterUnit(SerializationSettings)
    RegisterUnit(Foundation)
    RegisterUnit(AppLoop)
    RegisterUnit(CommandLineArgument)
    RegisterUnit(Invocation)
    RegisterUnit(Stream)
    RegisterUnit(MemoryStream)
    RegisterUnit(PersistancePoint)
    RegisterUnit(StopWatch)
    RegisterUnit(Task)
    RegisterUnit(Timer)
    RegisterUnit(ValueConverter)
    RegisterUnit(VirtualPath)
    RegisterUnit(Visibility)
    RegisterUnit(Invalidation)
EndModule()
