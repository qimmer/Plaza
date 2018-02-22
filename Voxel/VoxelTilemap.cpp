
#include "VoxelTilemap.h"


    struct VoxelTilemap {
    };

    DefineComponent(VoxelTilemap)

    EndComponent()

    DefineService(VoxelTilemap)

    EndService()

    static bool ServiceStart() {
        return true;
    }

    static bool ServiceStop() {
        return true;
    }
}