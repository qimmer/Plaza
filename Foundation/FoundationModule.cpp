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
#include "FoundationModule.h"

struct FoundationData {
    Entity TaskQueue, FilesystemUpdateTimer;
};

struct FoundationModule {
	Vector(FileTypes, Entity, 32)
};

BeginUnit(Foundation)
    BeginComponent(FoundationData)
        RegisterChildProperty(TaskQueue, TaskQueue)
    EndComponent()
	BeginComponent(FoundationModule)
		RegisterArrayProperty(FileType, FileTypes)
	EndComponent()
EndUnit()

BeginModule(Foundation, ComponentOf_FoundationData())
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
