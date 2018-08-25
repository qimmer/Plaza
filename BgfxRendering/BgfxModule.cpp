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
        },
        "Extensions": [
           {
                "ExtensionDisabled": true,
                "ExtensionComponent": "/Modules/Rendering/Components/CommandList",
                "ExtensionExtenderComponent": "/Modules/BgfxRendering/Components/BgfxCommandList"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "/Modules/Rendering/Components/VertexDeclaration",
               "ExtensionExtenderComponent": "/Modules/BgfxRendering/Components/BgfxVertexDeclaration"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "/Modules/Rendering/Components/VertexBuffer",
               "ExtensionExtenderComponent": "/Modules/BgfxRendering/Components/BgfxVertexBuffer"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "/Modules/Rendering/Components/IndexBuffer",
               "ExtensionExtenderComponent": "/Modules/BgfxRendering/Components/BgfxIndexBuffer"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "/Modules/Rendering/Components/OffscreenRenderTarget",
               "ExtensionExtenderComponent": "/Modules/BgfxRendering/Components/BgfxOffscreenRenderTarget"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "/Modules/Rendering/Components/BinaryProgram",
               "ExtensionExtenderComponent": "/Modules/BgfxRendering/Components/BgfxBinaryProgram"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "/Modules/Rendering/Components/Texture2D",
               "ExtensionExtenderComponent": "/Modules/BgfxRendering/Components/BgfxTexture2D"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "/Modules/Rendering/Components/Uniform",
               "ExtensionExtenderComponent": "/Modules/BgfxRendering/Components/BgfxUniform"
           }
        ]
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