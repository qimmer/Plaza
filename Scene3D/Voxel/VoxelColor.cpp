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

BeginUnit(VoxelColor)
    BeginComponent(VoxelColor)
    RegisterProperty(Voxel, VoxelColorValue)
    RegisterProperty(rgba8, VoxelColorColor)
    RegisterProperty(Entity, VoxelColorSubTextureNX)
    RegisterProperty(Entity, VoxelColorSubTexturePX)
    RegisterProperty(Entity, VoxelColorSubTextureNY)
    RegisterProperty(Entity, VoxelColorSubTexturePY)
    RegisterProperty(Entity, VoxelColorSubTextureNZ)
    RegisterProperty(Entity, VoxelColorSubTexturePZ)
EndComponent()
EndUnit()
(Voxel, VoxelColorValue)
RegisterProperty(rgba8, VoxelColorColor)
RegisterProperty(Entity, VoxelColorSubTextureNX)
RegisterProperty(Entity, VoxelColorSubTexturePX)
RegisterProperty(Entity, VoxelColorSubTextureNY)
RegisterProperty(Entity, VoxelColorSubTexturePY)
RegisterProperty(Entity, VoxelColorSubTextureNZ)
RegisterProperty(Entity, VoxelColorSubTexturePZ)
