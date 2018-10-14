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

BeginUnit(VoxelPalette)
    BeginComponent(VoxelPalette)
    RegisterBase(Material)
EndComponent()

DefineType(Voxel)
EndType()

static void UpdatePalette(Entity palette) {
    auto data = GetVoxelPalette(palette);
    Assert(StreamOpen(data->VoxelPaletteTexture, StreamMode_Write));
    for(auto voxelColor = GetFirstChild(palette); voxelColor; voxelColor = GetSibling(voxelColor)) {
        if(HasComponent(voxelColor, ComponentOf_VoxelColor())) {
            auto rgba = GetVoxelColorColor(voxelColor);
            auto offset = (u32)GetVoxelColorValue(voxelColor) * 4;
            StreamSeek(data->VoxelPaletteTexture, offset);
            StreamWrite(data->VoxelPaletteTexture, 4, &rgba);
        }
    }
    StreamClose(data->VoxelPaletteTexture);
}

LocalFunction(OnVoxelColorChanged, void, Entity voxelColor) {
    auto parent = GetParent(voxelColor);
    if(IsEntityValid(parent) && HasComponent(parent, ComponentOf_VoxelPalette())) {
        invalidatedPalettes.insert(parent);
    }
}

LocalFunction(OnParentChanged, void, Entity voxelColor, Entity oldPalette, Entity newPalette) {
    if(HasComponent(voxelColor, ComponentOf_VoxelColor())) {
        if(IsEntityValid(oldPalette) && HasComponent(oldPalette, ComponentOf_VoxelPalette())) {
            invalidatedPalettes.insert(oldPalette);
        }
        if(IsEntityValid(newPalette) && HasComponent(newPalette, ComponentOf_VoxelPalette())) {
            invalidatedPalettes.insert(newPalette);
        }
    }
}

LocalFunction(OnAdded, void, Entity voxelPalette) {
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

LocalFunction(OnAppUpdate, void, double deltaTime) {
    for(auto palette : invalidatedPalettes) {
        UpdatePalette(palette);
    }
    invalidatedPalettes.clear();
}

DefineService(VoxelPalette)
    ServiceDependency(VoxelMesh)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_VoxelColor()), OnVoxelColorChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Parent()), OnParentChanged, 0)
    RegisterSubscription(VoxelPaletteAdded, OnAdded, 0)
    RegisterSubscription(AppUpdate, OnAppUpdate, 0)
EndService()
