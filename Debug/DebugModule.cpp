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
#include "FlowNode.h"
#include "ModuleProfiling.h"

struct DebugSession {
    Entity DebugSessionChangeTracker;
};

struct Debug {
    Entity DebugServer;
    ChildArray DebugServerSessions;
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

    if(!StreamOpen(responseStream, StreamMode_Write)) {
        return 500;
    }

    StreamWrite(responseStream, strlen(json), json);
    StreamClose(responseStream);

    auto session = CreateEntity();
    //SetName(session, guid);
    SetStopWatch(session, {true, 0.0});

    auto debugData = GetDebug(ModuleOf_Debug());
    debugData.DebugServerSessions.Add(session);
    SetDebug(ModuleOf_Debug(), debugData);

    return 200;
}

BeginUnit(DebugModule)
    RegisterFunctionSignature(NativeFunctionInvoker_u16_Entity_StringRef, u16, Entity, StringRef)
    RegisterFunction(DebugAddComponent)
    RegisterFunction(DebugRemoveComponent)
    RegisterFunction(DebugCreateSession)

    BeginComponent(DebugServer)
        RegisterBase(HttpServer)
    EndComponent()

    BeginComponent(DebugSession)
        BeginChildProperty(DebugSessionChangeTracker)
    EndChildProperty()
    EndComponent()

    BeginComponent(Debug)
        BeginChildProperty(DebugServer)
        EndChildProperty()
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
                "ServerPort": 8080,
                "HttpServerKeepAliveTimeout": 5,
                "HttpServerKeepAliveMaxConnections": 100,
                "RestServerRoutes": [
                    {
                        "RestResourceRoutingRoot": "file://Debug/wwwroot",
                        "RestRoutingRoute": "/"
                    },
                    {
                        "RestResourceRoutingRoot": "file://",
                        "RestRoutingRoute": "/asset/"
                    },
                    {
                        "RestRoutingRoute": "/api/entity",
                        "RestEntityRoutingJsonSettings": {
                            "JsonSettingsResolveBindings": true,
                            "JsonSettingsIgnoreReadOnly": false,
                            "JsonSettingsMaxRecursiveLevels": 0,
                            "JsonSettingsExplicitComponents": false
                        }
                    },
                    {
                        "RestFunctionRoutingFunction": "Function.DebugCreateSession",
                        "RestRoutingRoute": "/api/session"
                    },
                    {
                        "RestFunctionRoutingFunction": "Function.DebugGetChanges",
                        "RestRoutingRoute": "/api/changes"
                    },
                    {
                        "RestFunctionRoutingFunction": "Function.DebugAddComponent",
                        "RestRoutingRoute": "/api/addcomponent"
                    },
                    {
                        "RestFunctionRoutingFunction": "Function.DebugRemoveComponent",
                        "RestRoutingRoute": "/api/removecomponent"
                    }
                ]
            }
		}
	);

    Info(module, "\n--- Debugging Server is now listening on port 8080 ---\n");
EndUnit()

BeginModule(Debug)
    RegisterDependency(Core)
    RegisterDependency(Foundation)

    RegisterUnit(ModuleProfiling)

    RegisterDependency(File)
    RegisterDependency(Networking)
    RegisterDependency(Json)
    RegisterDependency(Rest)

    RegisterUnit(DebugModule)
    RegisterUnit(FlowNode)
EndModule()