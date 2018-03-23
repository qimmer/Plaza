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
    DefineProperty(Voxel, VoxelColorValue)
    DefineProperty(rgba8, VoxelColorColor)
    DefineProperty(Entity, VoxelColorSubTextureNX)
    DefineProperty(Entity, VoxelColorSubTexturePX)
    DefineProperty(Entity, VoxelColorSubTextureNY)
    DefineProperty(Entity, VoxelColorSubTexturePY)
    DefineProperty(Entity, VoxelColorSubTextureNZ)
    DefineProperty(Entity, VoxelColorSubTexturePZ)
EndComponent()

DefineComponentPropertyReactive(VoxelColor, Voxel, VoxelColorValue)
DefineComponentPropertyReactive(VoxelColor, rgba8, VoxelColorColor)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNX)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePX)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNY)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePY)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNZ)
DefineComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePZ)
