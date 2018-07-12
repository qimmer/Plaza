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
#include "Mesh.h"
#include "Material.h"
#include "UniformState.h"
#include "Context.h"
#include "VertexAttribute.h"
#include "SubTexture2D.h"
#include "VertexDeclaration.h"
#include "OffscreenRenderTarget.h"
#include <Foundation/FoundationModule.h>
#include <Input/InputModule.h>

BeginModule(Rendering)
    ModuleDependency(Foundation)
    ModuleDependency(Input)

    ModuleService(ShaderCompiler)
    ModuleService(Program)
    ModuleService(Texture2D)
    ModuleService(SubTexture2D)
    ModuleService(OffscreenRenderTarget)

    ModuleType(VertexDeclaration)
    ModuleType(VertexAttribute)
    ModuleType(VertexBuffer)
    ModuleType(IndexBuffer)
    ModuleType(Context)
    ModuleType(Uniform)
    ModuleType(UniformState)
    ModuleType(Texture)
    ModuleType(Texture2D)
    ModuleType(RenderTarget)
    ModuleType(Shader)
    ModuleType(BinaryShader)
    ModuleType(Program)
    ModuleType(Mesh)
    ModuleType(Material)
EndModule()
