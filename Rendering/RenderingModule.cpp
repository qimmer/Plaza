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
#include "SceneRenderer.h"
#include "OffscreenRenderTarget.h"
#include <Foundation/FoundationModule.h>
#include <Input/InputModule.h>
#include <Core/NativeUtils.h>
#include <Json/NativeUtils.h>
#include <Scene/SceneModule.h>
#include <Foundation/AppNode.h>
#include <Foundation/Stream.h>

BeginModule(Rendering)
    RegisterDependency(Foundation)
    RegisterDependency(Input)
    RegisterDependency(Scene)

    RegisterUnit(Material)
    RegisterUnit(Mesh)
    RegisterUnit(OffscreenRenderTarget)
    RegisterUnit(Program)
    RegisterUnit(RenderContext)
    RegisterUnit(Renderable)
    RegisterUnit(RenderState)
    RegisterUnit(RenderTarget)
    RegisterUnit(ShaderCache)
    RegisterUnit(Texture)
    RegisterUnit(Texture2D)
    RegisterUnit(Uniform)
    RegisterUnit(SceneRenderer)
    RegisterUnit(MeshBuilder)
    RegisterUnit(Rendering)

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
                    { "FileTypeExtension": ".psb", "FileTypeMimeType" : "application/binary-pixel-shader" },
                    { "FileTypeExtension": ".vtb", "FileTypeMimeType" : "application/vertex-buffer" },
                    { "FileTypeExtension": ".idb", "FileTypeMimeType" : "application/index-buffer" }
                ]
            }
    );

    auto whiteTexture = TextureOf_White();
    SetTextureSize2D(whiteTexture, {1, 1});
    SetTextureFormat(whiteTexture, TextureFormat_RGBA8);
    SetStreamPath(whiteTexture, "memory://Rendering/WhiteTexture.bin");

    if(StreamOpen(whiteTexture, StreamMode_Write)) {
        rgba8 white;
        white.rgba = 0xffffffff;

        StreamWrite(whiteTexture, 4, &white.rgba);
        StreamClose(whiteTexture);
    }
EndUnit()
