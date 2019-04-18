//
// Created by Kim on 16/07/2018.
//

#include <Test/Test.h>
#include <Test/TestModule.h>
#include <Foundation/FoundationModule.h>
#include <File/FileModule.h>
#include <Json/JsonModule.h>
#include <Networking/NetworkingModule.h>
#include <Foundation/NativeUtils.h>
#include <Foundation/AppLoop.h>
#include <Networking/Server.h>
#include <Networking/HttpRequest.h>
#include <Foundation/MemoryStream.h>
#include <Json/JsonPersistance.h>
#include <Networking/HttpServer.h>
#include <Core/Identification.h>
#include <Core/Debug.h>
#include "TestNetworking.h"

Test(TestNetworking) {
    auto server = CreateEntity();
    AddComponent(server, ComponentOf_HttpServer());
    SetServerPort(server, 8000);

    auto appLoop = CreateEntity();
    AddComponent(appLoop, ComponentOf_AppLoop());
    RunAppLoop(appLoop);

    return Success;
}

static void OnHttpServerRequest(Entity server, Entity request) {
    auto response = GetHttpRequestResponse(request);
    SetStreamPath(response, "memory://response.json");
    auto url = GetHttpRequestUrl(request);
    auto root = FindEntityByPath(url);

    if(IsEntityValid(root)) {
        SerializeJson(response, root);
        SetHttpResponseCode(response, 200);
        SetStreamPath(response, "memory://response.json");
    }
}

BeginModule(TestNetworking)
    RegisterDependency(Core)
    RegisterDependency(Test)
    RegisterDependency(Foundation)
    RegisterDependency(File)
    RegisterDependency(Json)
    RegisterDependency(Networking)
    RegisterUnit(TestNetworking)
EndModule()

BeginUnit(TestNetworking)
    RegisterTest(TestNetworking)
    RegisterSubscription(HttpServerRequest, OnHttpServerRequest, 0)
EndUnit()
