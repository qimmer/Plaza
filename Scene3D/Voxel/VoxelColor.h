//
// Created by Kim Johannsen on 13-03-2018.
//

#ifndef PLAZA_VOXELCOLOR_H
#define PLAZA_VOXELCOLOR_H

#include <Core/NativeUtils.h>
#include "VoxelPalette.h"

Unit(VoxelColor)
    Component(VoxelColor)
        Property(Voxel, VoxelColorValue)
        Property(rgba8, VoxelColorColor)
        Property(Entity, VoxelColorSubTextureNX)
        Property(Entity, VoxelColorSubTexturePX)
        Property(Entity, VoxelColorSubTextureNY)
        Property(Entity, VoxelColorSubTexturePY)
        Property(Entity, VoxelColorSubTextureNZ)
        Property(Entity, VoxelColorSubTexturePZ)

#endif //PLAZA_VOXELCOLOR_H
