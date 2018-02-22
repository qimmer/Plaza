
#ifndef VoxelMesh_H
#define VoxelMesh_H

#include <Core/Entity.h>


    DeclareComponent(VoxelMesh)
    DeclareService(VoxelMesh)

    DeclareComponentProperty(VoxelMesh, Entity, VoxelDataStream)
    DeclareComponentProperty(VoxelMesh, Entity, VoxelMeshTilemap)
    DeclareComponentProperty(VoxelMesh, v3i, VoxelMeshSize)

#endif //VoxelMesh_H
