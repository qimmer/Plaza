//
// Created by Kim on 27-07-2018.
//

#include <Foundation/NativeUtils.h>
#include <Foundation/FoundationModule.h>
#include <Networking/NetworkingModule.h>
#include <Networking/Server.h>
#include <Networking/HttpServer.h>
#include <Json/JsonModule.h>
#include <Json/NativeUtils.h>
#include <Rest/RestModule.h>
#include <File/FileModule.h>
#include <cstdarg>
#include <Core/Identification.h>
#include <Foundation/StopWatch.h>
#include "DebugModule.h"
#include "EntityTracker.h"
#include "FlowNode.h"

struct DebugSession {
    Entity DebugSessionChangeTracker;
};

struct Debug {
    Entity DebugServer;
};

struct DebugServer {
};

static u16 DebugComponent(StringRef path, bool(*func)(Entity, Entity)) {
    path += 1; // Discard initial slash

    auto slashLoc = strchr(path, '/');
    if(!slashLoc) {
        return 405;
    }

    auto uuid = slashLoc + 1;

    auto componentUuidLength = (size_t)slashLoc - (size_t)path;
    char componentUuid[128];
    Assert(0, componentUuidLength < 128);
    memcpy(componentUuid, path, componentUuidLength);
    componentUuid[componentUuidLength] = '\0';

    auto entity = FindEntityByUuid(uuid);
    if(!IsEntityValid(entity)) {
        return 404;
    }

    auto component = FindEntityByUuid(componentUuid);
    if(!IsEntityValid(component)) {
        return 404;
    }

    if(!func(entity, component)) return 304; // Not modified

    return 200;
}

NativeFunctionInvoker2(u16, Entity, StringRef)

u16 DebugAddComponent(Entity responseStream, StringRef path) {
    return DebugComponent(path, AddComponent);
}

u16 DebugRemoveComponent(Entity responseStream, StringRef path) {
    return DebugComponent(path, RemoveComponent);
}

u16 DebugCreateSession(Entity responseStream, StringRef path) {
    auto guid = CreateGuid();
    char json[128];
    snprintf(json, 128, "\"%s\"", guid);

    SetStreamPath(responseStream, "memory://response.json");
    if(!StreamOpen(responseStream, StreamMode_Write)) {
        return 500;
    }

    StreamWrite(responseStream, strlen(json), json);
    StreamClose(responseStream);

    auto session = AddDebugServerSessions(ModuleOf_Debug());
    SetName(session, guid);
    SetStopWatchRunning(session, true);

    return 200;
}

u16 DebugGetChanges(Entity responseStream, StringRef path) {
    if(path[0] != '/') return 400;

    char sessionPath[1024];
    snprintf(sessionPath, 1024, "/Modules/Debug/DebugServerSessions%s", path);

    auto session = FindEntityByPath(sessionPath);
    if(!IsEntityValid(session)) {
        return 404;
    }

    return GetEntityTrackerChanges(GetDebugSessionChangeTracker(session), responseStream);
}

BeginUnit(DebugModule)
    RegisterFunctionSignature(NativeFunctionInvoker_u16_Entity_StringRef, u16, Entity, StringRef)
    RegisterFunction(DebugAddComponent)
    RegisterFunction(DebugRemoveComponent)
    RegisterFunction(DebugGetChanges)
    RegisterFunction(DebugCreateSession)

    BeginComponent(DebugServer)
        RegisterBase(HttpServer)
    EndComponent()

    BeginComponent(DebugSession)
        RegisterChildProperty(EntityTracker, DebugSessionChangeTracker)
    EndComponent()

    BeginComponent(Debug)
        RegisterChildProperty(DebugServer, DebugServer)
        RegisterArrayProperty(DebugSession, DebugServerSessions)
    EndComponent()

	ModuleData(
		{
			"FileTypes": [
				{ "FileTypeExtension": ".html", "FileTypeMimeType" : "text/html" },
				{ "FileTypeExtension": ".css", "FileTypeMimeType" : "text/css" },
                { "FileTypeExtension": ".woff", "FileTypeMimeType" : "font/woff" },
                { "FileTypeExtension": ".woff2", "FileTypeMimeType" : "font/woff2" },
                { "FileTypeExtension": ".ttf", "FileTypeMimeType" : "font/ttf" },
				{ "FileTypeExtension": ".map", "FileTypeMimeType" : "text/map" },
				{ "FileTypeExtension": ".js", "FileTypeMimeType" : "application/javascript" }
			],
            "DebugServer":{
                "Name": "Debug Server",
                "ServerPort": 8080,
                "HttpServerKeepAliveTimeout": 5,
                "HttpServerKeepAliveMaxConnections": 100,
                "RestServerRoutes": [
                    {
                        "Name": "WebRoute",
                        "RestResourceRoutingRoot": "file://Debug/wwwroot",
                        "RestRoutingRoute": "/"
                    },
                    {
                        "Name": "EntityRoute",
                        "RestEntityRoutingRoot": "/",
                        "RestRoutingRoute": "/api/entity",
                        "RestEntityRoutingDepth": 4
                    },
                    {
                        "Name": "SessionRoute",
                        "RestFunctionRoutingFunction": "/Modules/Debug/Functions/DebugCreateSession",
                        "RestRoutingRoute": "/api/session"
                    },
                    {
                        "Name": "ChangesRoute",
                        "RestFunctionRoutingFunction": "/Modules/Debug/Functions/DebugGetChanges",
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
    RegisterUnit(DebugModule)
    RegisterUnit(FlowNode)
EndModule()