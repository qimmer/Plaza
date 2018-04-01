//
// Created by Kim Johannsen on 13-03-2018.
//

#include "VoxelColor.h"
#include "VoxelPalette.h"

struct VoxelColor {
    Voxel VoxelColorValue;
    Entity VoxelColorSubTextureNX, VoxelColorSubTexturePX, VoxelColorSubTextureNY, VoxelColorSubTexturePY, VoxelColorSubTextureNZ, VoxelColorSubTexturePZ;
    rgba8 VoxelColorColor;
};

DefineComponent(VoxelColor)
    DefinePropertyReactive(Voxel, VoxelColorValue)
    DefinePropertyReactive(rgba8, VoxelColorColor)
    DefinePropertyReactive(Entity, VoxelColorSubTextureNX)
    DefinePropertyReactive(Entity, VoxelColorSubTexturePX)
    DefinePropertyReactive(Entity, VoxelColorSubTextureNY)
    DefinePropertyReactive(Entity, VoxelColorSubTexturePY)
    DefinePropertyReactive(Entity, VoxelColorSubTextureNZ)
    DefinePropertyReactive(Entity, VoxelColorSubTexturePZ)
EndComponent()

DefineComponentPropertyReactive(VoxelColor, Voxel, VoxelColorValue)
DefineComponentPropertyReactive(VoxelColor, rgba8, VoxelColorColor)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNX)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePX)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNY)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePY)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNZ)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePZ)
