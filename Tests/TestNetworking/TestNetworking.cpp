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
#include "TestNetworking.h"

Test(TestNetworking) {

    auto server = CreateEntityFromName(test, "Server");
    AddComponent(server, ComponentOf_HttpServer());
    SetServerPort(server, 8000);

    auto appLoop = CreateEntityFromName(test, "AppLoop");
    AddComponent(appLoop, ComponentOf_AppLoop());
    RunAppLoop(appLoop);

    return Success;
}

LocalFunction(OnHttpServerRequest, void, Entity server, Entity request) {
    auto response = CreateEntityFromName(request, "Response");
    SetHttpResponseCode(response, 200);
    SetStreamPath(response, "memory://response.json");

    auto url = GetHttpRequestUrl(request);
    auto root = FindEntityByPath(url);

    if(IsEntityValid(root)) {
        SerializeJson(response, root);
        SetHttpRequestResponse(request, response);
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
