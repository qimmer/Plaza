//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_TESTCUBE_H
#define PLAZA_TESTCUBE_H

#include <Test/Test.h>
#include <Core/Entity.h>
#include <Foundation/AppLoop.h>
#include <Core/Module.h>
#include <Bgfx/BgfxModule.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/CommandList.h>
#include <Rendering/Context.h>
#include "Common.h"

using namespace Plaza;

TestResult TestCube()
{
    InitializeModule(ModuleOf_Bgfx());

    auto context = CreateContext();
    auto commandList = CreateCommandList();

    SetCommandListClearColor(commandList, {0.2f, 0.6f, 1.0f, 1.0f});
    SetCommandListClearTargets(commandList, ClearTarget_Color);

    SetContextSize(context, {800, 600});
    SetContextTitle(context, "Cube Test");

    SubscribeContextClosing(DestroyEntity);

    while(IsEntityValid(context)) {
        auto size = GetContextSize(context);
        SetCommandListViewport(commandList, {0, 0, size.x, size.y});
        AppUpdate();
    }

    UnsubscribeContextClosing(DestroyEntity);

    DestroyEntity(commandList);

    return Success;
}

#endif //PLAZA_TESTCUBE_H
