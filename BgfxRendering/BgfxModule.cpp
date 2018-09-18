//
// Created by Kim Johannsen on 15/01/2018.
//

#include <Json/NativeUtils.h>
#include <Foundation/AppLoop.h>

#include "BgfxModule.h"
#include "BgfxResource.h"
#include "BgfxRenderContext.h"
#include "BgfxCommandList.h"
#include "BgfxUniform.h"
#include "BgfxTexture2D.h"
#include "BgfxMesh.h"
#include "BgfxShaderCompiler.h"
#include "BgfxShaderCache.h"
#include "BgfxOffscreenRenderTarget.h"

#include <Core/CoreModule.h>
#include <Foundation/FoundationModule.h>
#include <Json/JsonModule.h>
#include <Rendering/RenderingModule.h>

BeginModule(BgfxRendering)
    RegisterDependency(Rendering)
    RegisterDependency(Json)
    RegisterDependency(Foundation)
    RegisterDependency(Core)

    RegisterUnit(BgfxShaderCompiler)
    RegisterUnit(BgfxRenderContext)
    RegisterUnit(BgfxCommandList)
    RegisterUnit(BgfxUniform)
    RegisterUnit(BgfxTexture2D)
    RegisterUnit(BgfxMesh)
    RegisterUnit(BgfxResource)
    RegisterUnit(BgfxShaderCache)
    RegisterUnit(BgfxOffscreenRenderTarget)
    RegisterUnit(BgfxRendering)

#ifdef BGFXRENDERING_WITH_SHADER_COMPILER
    RegisterUnit(BgfxShaderCompiler)
#endif

    ModuleData({
        "BgfxRenderingLoop": {
            "AppLoopDisabled": true
        }
    })
EndModule()

struct BgfxRendering {
    Entity BgfxRenderingLoop;
};

BeginUnit(BgfxRendering)
    BeginComponent(BgfxRendering)
        RegisterChildProperty(AppLoop, BgfxRenderingLoop)
    EndComponent()
EndUnit()