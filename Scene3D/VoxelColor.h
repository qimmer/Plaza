//
// Created by Kim Johannsen on 13-03-2018.
//

#ifndef PLAZA_VOXELCOLOR_H
#define PLAZA_VOXELCOLOR_H

#include <Core/Entity.h>
#include "VoxelPalette.h"

DeclareComponent(VoxelColor)
DeclareComponentPropertyReactive(VoxelColor, Voxel, VoxelColorValue)
DeclareComponentPropertyReactive(VoxelColor, rgba8, VoxelColorColor)
DeclareComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNX)
DeclareComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePX)
DeclareComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNY)
DeclareComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePY)
DeclareComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTextureNZ)
DeclareComponentPropertyReactive(VoxelColor, Entity, VoxelColorSubTexturePZ)

#endif //PLAZA_VOXELCOLOR_H
