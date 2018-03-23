//
// Created by Kim Johannsen on 13-03-2018.
//

#include <Rendering/Material.h>
#include <Rendering/UniformState.h>
#include <Rendering/Uniform.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include <Foundation/Stream.h>
#include "VoxelPalette.h"
#include "VoxelColor.h"
#include "VoxelMesh.h"

struct VoxelPalette {
    Entity VoxelPaletteTextureUniformState;
    Entity VoxelPaletteTexture;
};

DefineComponent(VoxelPalette)
    Dependency(Material)
EndComponent()

DefineType(Voxel)
EndType()

static void OnVoxelColorChanged(Entity voxelColor) {
    auto parent = GetParent(voxelColor);
    if(IsEntityValid(parent) && HasVoxelPalette(parent)) {
        auto data = GetVoxelPalette(parent);
        auto rgba = GetVoxelColorColor(voxelColor);

        Assert(StreamOpen(data->VoxelPaletteTexture, StreamMode_Read));
        StreamSeek(data->VoxelPaletteTexture, GetVoxelColorValue(voxelColor) * 4);
        StreamWrite(data->VoxelPaletteTexture, 4, &rgba);
        StreamClose(data->VoxelPaletteTexture);
    }
}

static void OnParentChanged(Entity voxelColor, Entity oldPalette, Entity newPalette) {
    if(HasVoxelColor(voxelColor) && IsEntityValid(newPalette) && HasVoxelPalette(newPalette)) {
        OnVoxelColorChanged(voxelColor);
    }
}

static void OnAdded(Entity voxelPalette) {
    auto data = GetVoxelPalette(voxelPalette);

    auto us = CreateUniformState(voxelPalette, "TextureUniformState");
    SetUniformStateUniform(us, GetVoxelPaletteTextureUniform());

    auto tex = CreateTexture2D(voxelPalette, "ColorsTexture");
    SetTextureSize2D(tex, {256, 1});
    SetTextureFormat(tex, TextureFormat_RGBA8);
    SetTextureFlag(tex, TextureFlag_MIN_POINT | TextureFlag_MAG_POINT);

    SetUniformStateTexture(us, tex);

    data->VoxelPaletteTextureUniformState = us;
    data->VoxelPaletteTexture = tex;

    SetMaterialProgram(voxelPalette, GetVoxelProgram());
}

DefineService(VoxelPalette)
    ServiceDependency(VoxelMesh)
    Subscribe(VoxelColorChanged, OnVoxelColorChanged)
    Subscribe(ParentChanged, OnParentChanged)
    Subscribe(VoxelPaletteAdded, OnAdded)
EndService()

static bool ServiceStart() {

    return true;
}

static bool ServiceStop() {
    return true;
}
