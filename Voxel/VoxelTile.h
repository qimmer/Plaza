
#ifndef VoxelTile_H
#define VoxelTile_H

#include <Core/Entity.h>


    typedef void(*VoxelTileMeshGeneratorHandler)(Entity voxelMesh, Entity voxelTile);

    DeclareComponent(VoxelTile)
    DeclareService(VoxelTile)

    DeclareComponentProperty(VoxelTile, v2f, VoxelTileUvOffset)
    DeclareComponentProperty(VoxelTile, v2f, VoxelTileUvScale)
    DeclareComponentProperty(VoxelTile, u16, VoxelTileValue)
    DeclareComponentProperty(VoxelTile, VoxelTileMeshGeneratorHandler, VoxelTileMeshGenerator)

#endif //VoxelTile_H
