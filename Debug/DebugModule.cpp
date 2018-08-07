//
// Created by Kim on 27-07-2018.
//

#include <Foundation/NativeUtils.h>
#include <Foundation/FoundationModule.h>
#include <Networking/NetworkingModule.h>
#include <Json/JsonModule.h>
#include <Json/NativeUtils.h>
#include <Rest/RestModule.h>
#include <File/FileModule.h>
#include "DebugModule.h"
#include "EntityTracker.h"

struct Debug {
    Entity EntityTracker;
};

BeginUnit(DebugServer)
    BeginComponent(Debug)
        RegisterChildProperty(EntityTracker, EntityTracker)
    EndComponent()

	ModuleData(
		{
			"FileTypes": [
				{ "FileTypeExtension": ".html", "FileTypeMimeType" : "text/html" },
				{ "FileTypeExtension": ".css", "FileTypeMimeType" : "text/css" },
				{ "FileTypeExtension": ".map", "FileTypeMimeType" : "text/map" },
				{ "FileTypeExtension": ".js", "FileTypeMimeType" : "application/javascript" }
			]
		}
	);
EndUnit()

BeginModule(Debug)
    RegisterDependency(Core)
    RegisterDependency(Foundation)
    RegisterDependency(File)
    RegisterDependency(Networking)
    RegisterDependency(Json)
    RegisterDependency(Rest)
    RegisterUnit(DebugServer)
    RegisterUnit(EntityTracker)
    RegisterData("file://Debug.json")

    AddComponent(module, ComponentOf_Debug());
EndModule()