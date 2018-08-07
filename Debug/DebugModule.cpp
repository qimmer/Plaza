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
			],
            "Roots": [
                {
                    "Name": "Debug Server",
                    "ServerPort": 8080,
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
                        "RestRoutingRoute": "/api"
                    },
                    {
                        "Name": "ChangesRoute",
                        "RestFunctionRoutingFunction": "/Modules/Debug/Functions/GetChanges",
                        "RestRoutingRoute": "/changes"
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
EndModule()