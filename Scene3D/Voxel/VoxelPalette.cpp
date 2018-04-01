//
// Created by Kim Johannsen on 13-03-2018.
//

#include <Rendering/Material.h>
#include <Rendering/UniformState.h>
#include <Rendering/Uniform.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include "VoxelPalette.h"
#include "VoxelColor.h"
#include "VoxelMesh.h"

static Set<Entity> invalidatedPalettes;

struct VoxelPalette {
    Entity VoxelPaletteTextureUniformState;
    Entity VoxelPaletteTexture;
};

DefineComponent(VoxelPalette)
    Dependency(Material)
EndComponent()

DefineType(Voxel)
EndType()

static void UpdatePalette(Entity palette) {
    auto data = GetVoxelPalette(palette);
    Assert(StreamOpen(data->VoxelPaletteTexture, StreamMode_Write));
    for(auto voxelColor = GetFirstChild(palette); voxelColor; voxelColor = GetSibling(voxelColor)) {
        if(HasVoxelColor(voxelColor)) {
            auto rgba = GetVoxelColorColor(voxelColor);
            auto offset = (u32)GetVoxelColorValue(voxelColor) * 4;
            StreamSeek(data->VoxelPaletteTexture, offset);
            StreamWrite(data->VoxelPaletteTexture, 4, &rgba);
        }
    }
    StreamClose(data->VoxelPaletteTexture);
}

static void OnVoxelColorChanged(Entity voxelColor) {
    auto parent = GetParent(voxelColor);
    if(IsEntityValid(parent) && HasVoxelPalette(parent)) {
        invalidatedPalettes.insert(parent);
    }
}

static void OnParentChanged(Entity voxelColor, Entity oldPalette, Entity newPalette) {
    if(HasVoxelColor(voxelColor)) {
        if(IsEntityValid(oldPalette) && HasVoxelPalette(oldPalette)) {
            invalidatedPalettes.insert(oldPalette);
        }
        if(IsEntityValid(newPalette) && HasVoxelPalette(newPalette)) {
            invalidatedPalettes.insert(newPalette);
        }
    }
}

static void OnAdded(Entity voxelPalette) {
    auto data = GetVoxelPalette(voxelPalette);

    auto us = CreateUniformState(voxelPalette, "TextureUniformState");
    SetUniformStateUniform(us, GetVoxelPaletteTextureUniform());

    auto tex = CreateTexture2D(voxelPalette, "ColorsTexture");
    SetTextureSize2D(tex, {256, 1});
    SetTextureFormat(tex, TextureFormat_RGBA8);

    SetUniformStateTexture(us, tex);

    data->VoxelPaletteTextureUniformState = us;
    data->VoxelPaletteTexture = tex;

    SetMaterialVertexShader(voxelPalette, GetVoxelVertexShader());
    SetMaterialPixelShader(voxelPalette, GetVoxelPixelShader());
}

static void OnAppUpdate(double deltaTime) {
    for(auto palette : invalidatedPalettes) {
        UpdatePalette(palette);
    }
    invalidatedPalettes.clear();
}

DefineService(VoxelPalette)
    ServiceDependency(VoxelMesh)
    Subscribe(VoxelColorChanged, OnVoxelColorChanged)
    Subscribe(ParentChanged, OnParentChanged)
    Subscribe(VoxelPaletteAdded, OnAdded)
    Subscribe(AppUpdate, OnAppUpdate)
EndService()
