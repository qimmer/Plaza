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

	RegisterUnit(BgfxResource)
    RegisterUnit(BgfxShaderCompiler)
    RegisterUnit(BgfxRenderContext)
    RegisterUnit(BgfxCommandList)
    RegisterUnit(BgfxUniform)
    RegisterUnit(BgfxTexture2D)
    RegisterUnit(BgfxMesh)
    RegisterUnit(BgfxShaderCache)
    RegisterUnit(BgfxOffscreenRenderTarget)
    RegisterUnit(BgfxRendering)

#ifdef BGFXRENDERING_WITH_SHADER_COMPILER
    RegisterUnit(BgfxShaderCompiler)
#endif

    ModuleData({
        "Extensions": [
           {
                "ExtensionDisabled": true,
                "ExtensionComponent": "Component.CommandList",
                "ExtensionExtenderComponent": "Component.BgfxCommandList"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "Component.VertexDeclaration",
               "ExtensionExtenderComponent": "Component.BgfxVertexDeclaration"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "Component.VertexBuffer",
               "ExtensionExtenderComponent": "Component.BgfxVertexBuffer"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "Component.IndexBuffer",
               "ExtensionExtenderComponent": "Component.BgfxIndexBuffer"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "Component.OffscreenRenderTarget",
               "ExtensionExtenderComponent": "Component.BgfxOffscreenRenderTarget"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "Component.BinaryProgram",
               "ExtensionExtenderComponent": "Component.BgfxBinaryProgram"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "Component.Texture2D",
               "ExtensionExtenderComponent": "Component.BgfxTexture2D"
           },
           {
               "ExtensionDisabled": true,
               "ExtensionComponent": "Component.Uniform",
               "ExtensionExtenderComponent": "Component.BgfxUniform"
           },
           {
               "ExtensionDisabled": false,
               "ExtensionComponent": "Component.RenderContext",
               "ExtensionExtenderComponent": "Component.BgfxRenderContext"
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