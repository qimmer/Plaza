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
#include <cstdarg>
#include <Core/Identification.h>
#include "DebugModule.h"
#include "EntityTracker.h"
#include "FlowNode.h"

struct Debug {
    Entity EntityTracker;
};

static u16 DebugComponent(StringRef path, bool(*func)(Entity, Entity)) {
    path += 1; // Discard initial slash

    auto entityPath = strchr(path, '/');
    if(!entityPath) {
        return 405;
    }

    auto componentNameLength = (size_t)entityPath - (size_t)path;
    char componentName[128];
    Assert(0, componentNameLength < 128);
    memcpy(componentName, path, componentNameLength);
    componentName[componentNameLength] = '\0';

    auto entity = FindEntityByPath(entityPath);
    if(!IsEntityValid(entity)) {
        return 404;
    }

    auto component = FindEntityByName(ComponentOf_Component(), componentName);
    if(!IsEntityValid(component)) {
        return 404;
    }

    if(!func(entity, component)) return 304; // Not modified

    return 200;
}

u16 DebugAddComponent(Entity responseStream, StringRef path) {
    return DebugComponent(path, AddComponent);
}

u16 DebugRemoveComponent(Entity responseStream, StringRef path) {
    return DebugComponent(path, RemoveComponent);
}

BeginUnit(DebugServer)
    RegisterFunction(DebugAddComponent)
    RegisterFunction(DebugRemoveComponent)

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
			],
            "Roots": [
                {
                    "Name": "Debug Server",
                    "ServerPort": 8080,
                    "AppLoopKeepAlive": true,
                    "HttpServerKeepAliveTimeout": 5,
                    "HttpServerKeepAliveMaxConnections": 100,
                    "RestServerRoutes": [
                    {
                        "Name": "WebRoute",
                        "RestResourceRoutingRoot": "file://wwwroot",
                        "RestRoutingRoute": "/"
                    },
                    {
                        "Name": "EntityRoute",
                        "RestEntityRoutingRoot": "/",
                        "RestRoutingRoute": "/api/entity"
                    },
                    {
                        "Name": "ChangesRoute",
                        "RestFunctionRoutingFunction": "/Modules/Debug/Functions/GetChanges",
                        "RestRoutingRoute": "/api/changes"
                    },
                    {
                        "Name": "AddComponentRoute",
                        "RestFunctionRoutingFunction": "/Modules/Debug/Functions/DebugAddComponent",
                        "RestRoutingRoute": "/api/addcomponent"
                    },
                    {
                        "Name": "RemoveComponentRoute",
                        "RestFunctionRoutingFunction": "/Modules/Debug/Functions/DebugRemoveComponent",
                        "RestRoutingRoute": "/api/removecomponent"
                    }
                    ]
                }
            ],
            "EntityTracker": {
			    "EntityTrackerTrackValues": true
			}
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

    RegisterUnit(EntityTracker)
    RegisterUnit(DebugServer)
    RegisterUnit(FlowNode)
EndModule()