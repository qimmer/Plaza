//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_TESTCUBE_H
#define PLAZA_TESTCUBE_H

#include <Test/Test.h>
#include <Core/Entity.h>
#include <Foundation/AppLoop.h>
#include <Core/Module.h>
#include <BgfxRendering/BgfxModule.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/CommandList.h>
#include <Rendering/Context.h>
#include "Common.h"

TestResult TestCube()
{
    InitializeModule(ModuleOf_BgfxRendering());

    auto context = CreateContext(0, "CubeContext");
    auto commandList = CreateCommandList(0, "CubeContext/CommandList");

    SetCommandListClearColor(commandList, {50, 128, 255, 255});
    SetCommandListClearTargets(commandList, ClearTarget_Color);

    SetRenderTargetSize(context, {800, 600});
    SetContextTitle(context, "Cube Test");

    SubscribeContextClosing(context, DestroyEntity);

    while(IsEntityValid(context)) {
        auto size = GetRenderTargetSize(context);
        SetCommandListViewport(commandList, {0, 0, size.x, size.y});
        AppUpdate();
    }

    UnsubscribeContextClosing(context, DestroyEntity);

    DestroyEntity(commandList);

    return Success;
}

#endif //PLAZA_TESTCUBE_H
