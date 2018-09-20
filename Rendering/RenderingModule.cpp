//
// Created by Kim Johannsen on 08/01/2018.
//

#include "RenderingModule.h"
#include "ShaderCache.h"
#include "Program.h"
#include "Uniform.h"
#include "Texture.h"
#include "Texture2D.h"
#include "RenderTarget.h"
#include "RenderState.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderContext.h"
#include "SubTexture2D.h"
#include "Batch.h"
#include "CommandList.h"
#include "OffscreenRenderTarget.h"
#include <Foundation/FoundationModule.h>
#include <Input/InputModule.h>
#include <Core/NativeUtils.h>
#include <Json/NativeUtils.h>

BeginModule(Rendering)
    RegisterDependency(Foundation)
    RegisterDependency(Input)

    RegisterUnit(Batch)
    RegisterUnit(CommandList)
    RegisterUnit(Material)
    RegisterUnit(Mesh)
    RegisterUnit(OffscreenRenderTarget)
    RegisterUnit(Program)
    RegisterUnit(RenderContext)
    RegisterUnit(Rendering)
    RegisterUnit(RenderState)
    RegisterUnit(RenderTarget)
    RegisterUnit(ShaderCache)
    RegisterUnit(SubTexture2D)
    RegisterUnit(Texture)
    RegisterUnit(Texture2D)
    RegisterUnit(Uniform)

    ModuleData({
        "RenderingUvOffsetScaleUniform": {

        }
    })
EndModule()

struct Rendering {
    Entity RenderingUvOffsetScaleUniform;
};

BeginUnit(Rendering)
    BeginComponent(Rendering)
        RegisterChildProperty(Uniform, RenderingUvOffsetScaleUniform)
    EndComponent()
EndUnit()
