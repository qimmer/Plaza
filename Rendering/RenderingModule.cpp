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
                    { "Uuid": "FileType.VertexShader", "FileTypeExtension": ".vs", "FileTypeMimeType" : "application/vertex-shader" },
                    { "Uuid": "FileType.PixelShader", "FileTypeExtension": ".ps", "FileTypeMimeType" : "application/pixel-shader" },
                    { "Uuid": "FileType.DeclShader", "FileTypeExtension": ".var", "FileTypeMimeType" : "application/decl-shader" },
                    { "Uuid": "FileType.BinaryVertexShader", "FileTypeExtension": ".vsb", "FileTypeMimeType" : "application/binary-vertex-shader" },
                    { "Uuid": "FileType.BinaryPixelShader", "FileTypeExtension": ".psb", "FileTypeMimeType" : "application/binary-pixel-shader" },
                    { "Uuid": "FileType.VertexBuffer", "FileTypeExtension": ".vtb", "FileTypeMimeType" : "application/vertex-buffer" },
                    { "Uuid": "FileType.IndexBuffer", "FileTypeExtension": ".idb", "FileTypeMimeType" : "application/index-buffer" }
                ]
            }
    );

    auto whiteTexture = TextureOf_White();
    SetTextureSize2D(whiteTexture, {32, 32});
    SetTextureFormat(whiteTexture, TextureFormat_RGBA8);
    SetStreamPath(whiteTexture, "memory://Rendering/WhiteTexture.bin");

    if(StreamOpen(whiteTexture, StreamMode_Write)) {
        rgba8 white, black;
        white.rgba = 0xffffffff;
        black.rgba = 0xff000000;

        rgba8 pixels[32*32];
        rgba8 colors[] = { white, black };

        for(auto y = 0; y < 32; ++y) {
            for(auto x = 0; x < 32; ++x) {
                auto color = colors[(x/16 + y/16) % 2];
                pixels[y*32 + x] = color;
            }
        }

        StreamWrite(whiteTexture, 32*32*4, pixels);
        StreamClose(whiteTexture);
    }
EndUnit()
