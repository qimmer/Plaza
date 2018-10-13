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
#include "MeshBuilder.h"
#include "Material.h"
#include "Renderable.h"
#include "RenderContext.h"
#include "SubTexture2D.h"
#include "SceneRenderer.h"
#include "OffscreenRenderTarget.h"
#include <Foundation/FoundationModule.h>
#include <Input/InputModule.h>
#include <Core/NativeUtils.h>
#include <Json/NativeUtils.h>
#include <Scene/SceneModule.h>

BeginModule(Rendering)
    RegisterDependency(Foundation)
    RegisterDependency(Input)
    RegisterDependency(Scene)

    RegisterUnit(Material)
    RegisterUnit(Mesh)
    RegisterUnit(OffscreenRenderTarget)
    RegisterUnit(Program)
    RegisterUnit(RenderContext)
    RegisterUnit(Rendering)
    RegisterUnit(Renderable)
    RegisterUnit(RenderState)
    RegisterUnit(RenderTarget)
    RegisterUnit(ShaderCache)
    RegisterUnit(SubTexture2D)
    RegisterUnit(Texture)
    RegisterUnit(Texture2D)
    RegisterUnit(Uniform)
    RegisterUnit(SceneRenderer)
    RegisterUnit(MeshBuilder)

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

    ModuleData(
            {
                "FileTypes": [
                    { "FileTypeExtension": ".vs", "FileTypeMimeType" : "application/vertex-shader" },
                    { "FileTypeExtension": ".ps", "FileTypeMimeType" : "application/pixel-shader" },
                    { "FileTypeExtension": ".var", "FileTypeMimeType" : "application/decl-shader" },
                    { "FileTypeExtension": ".vsb", "FileTypeMimeType" : "application/binary-vertex-shader" },
                    { "FileTypeExtension": ".psb", "FileTypeMimeType" : "application/binary-pixel-shader" }
                ]
            }
    );
EndUnit()
