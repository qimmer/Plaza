//
// Created by Kim Johannsen on 15/01/2018.
//

#include "BgfxModule.h"
#include "BgfxContext.h"
#include "BgfxCommandList.h"
#include "BgfxUniform.h"
#include "BgfxTexture2D.h"
#include "BgfxVertexBuffer.h"
#include "BgfxIndexBuffer.h"
#include "BgfxShaderCompiler.h"
#include "BgfxProgram.h"
#include "BgfxBinaryShader.h"
#include "BgfxVertexDeclaration.h"


DefineModule(BgfxRendering)
        ModuleService(BgfxCommandList)
        ModuleService(BgfxContext)
        ModuleService(BgfxUniform)
        ModuleService(BgfxTexture2D)
        ModuleService(BgfxVertexBuffer)
        ModuleService(BgfxVertexDeclaration)
        ModuleService(BgfxIndexBuffer)
        ModuleService(BgfxBinaryShader)
        ModuleService(BgfxProgram)

        ModuleType(BgfxCommandList)
        ModuleType(BgfxContext)
        ModuleType(BgfxUniform)
        ModuleType(BgfxTexture2D)
        ModuleType(BgfxVertexBuffer)
        ModuleType(BgfxVertexDeclaration)
        ModuleType(BgfxIndexBuffer)
        ModuleType(BgfxBinaryShader)
        ModuleType(BgfxProgram)

#ifdef BGFXRENDERING_WITH_SHADER_COMPILER
        ModuleService(BgfxShaderCompiler)
#endif
    EndModule()

