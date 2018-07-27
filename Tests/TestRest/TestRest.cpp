//
// Created by Kim on 17-07-2018.
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
#include <Rest/RestRouting.h>
#include <Rest/RestEntityRouting.h>
#include <Rest/RestResourceRouting.h>
#include <Rest/RestModule.h>
#include "TestRest.h"

Test(TestRest) {
    RunAppLoop(AddTestEntities(test));

    return Success;
}

BeginModule(TestRest)
    RegisterDependency(Core)
    RegisterDependency(Test)
    RegisterDependency(Foundation)
    RegisterDependency(File)
    RegisterDependency(Json)
    RegisterDependency(Networking)
    RegisterDependency(Rest)
    RegisterUnit(TestRest)
EndModule()

BeginUnit(TestRest)
    RegisterTest(TestRest)

    RegisterData("file://TestRest.json")
EndUnit()
