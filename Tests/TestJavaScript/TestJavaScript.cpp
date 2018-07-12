//
// Created by Kim on 12/07/2018.
//

#include <Core/CoreModule.h>
#include <Foundation/FoundationModule.h>
#include <JavaScript/JavaScriptModule.h>
#include <Test/Test.h>
#include <Foundation/Stream.h>
#include <Foundation/MemoryStream.h>
#include <JavaScript/JavaScript.h>
#include "TestJavaScript.h"

static char answer[128];

LocalFunction(OnScriptMessage, void, StringRef text) {
    snprintf(answer, 128, text);
}

Test(TestJavaScript) {
    auto nativeMessage = FindEntityByPath("/TestJavaScript/NativeMessage");
    auto scriptMessage = FindEntityByPath("/TestJavaScript/ScriptMessage");

    auto subscription = CreateEntityFromName(FunctionOf_TestJavaScript(), "scriptMessageSubscription");
    SetSubscriptionEvent(subscription, scriptMessage);
    SetSubscriptionHandler(subscription, FunctionOf_OnScriptMessage());

    snprintf(answer, 128, "No answer received from script :-(");

    FireEvent(nativeMessage, 0, "Hello from Native!");

    Verify(strcmp(answer, "Hello from JavaScript!") == 0, "JavaScript did not fire ScriptMessage event.");

    return Success;
}

BeginModule(TestJavaScript)
    RegisterDependency(Core)
    RegisterDependency(Foundation)
    RegisterDependency(JavaScript)
    RegisterUnit(TestJavaScript)

    RegisterData("test.js")
    RegisterData("test.json")
EndModule()

BeginUnit(TestJavaScript)
    RegisterTest(TestJavaScript)
EndUnit()
