
#include "VoxelMesh.h"
#include "VoxelTilemap.h"
#include "VoxelTile.h"
#include <Foundation/Stream.h>

#define
#include "stb_voxel_render.h"


    struct VoxelMesh {
        Entity VoxelDataStream, VoxelMeshTilemap;
        v3i VoxelMeshSize;
    };

    static stbvox_mesh_maker mm;

    DefineComponent(VoxelMesh)
        DefineProperty(Entity, VoxelDataStream)
        DefineProperty(Entity, VoxelMeshTilemap)
        DefineProperty(v3i, VoxelMeshSize)
    EndComponent()

    DefineService(VoxelMesh)

    EndService()

    DefineComponentProperty(VoxelMesh, Entity, VoxelDataStream)
    DefineComponentProperty(VoxelMesh, Entity, VoxelMeshTilemap)
    DefineComponentProperty(VoxelMesh, v3i, VoxelMeshSize)

    static void BuildMesh(Entity voxelMesh) {
        stbvox_set_buffer(&mm, )
    }

    static bool ServiceStart() {
        stbvox_init_mesh_maker(&mm);
        stbvox_set_input_stride(&mm, )
        return true;
    }

    static bool ServiceStop() {
        return true;
    }
}