//
// Created by Kim Johannsen on 08/01/2018.
//

#include "RenderingModule.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BinaryShader.h"
#include "ShaderCompiler.h"
#include "Shader.h"
#include "Program.h"
#include "Uniform.h"
#include "Texture.h"
#include "Texture2D.h"
#include "RenderTarget.h"
#include <Foundation/FoundationModule.h>


    DefineModule(Rendering)
        ModuleDependency(Foundation)
        ModuleService(VertexBuffer)
        ModuleService(IndexBuffer)
        ModuleService(BinaryShader)
        ModuleService(Shader)
        ModuleService(ShaderCompiler)
        ModuleService(Program)

        ModuleType(VertexDeclaration)
        ModuleType(VertexBuffer)
        ModuleType(IndexBuffer)
        ModuleType(Uniform)
        ModuleType(Texture)
        ModuleType(Texture2D)
        ModuleType(RenderTarget)
        ModuleType(Shader)
        ModuleType(BinaryShader)
        ModuleType(Program)
    EndModule()
