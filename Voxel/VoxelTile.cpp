
#include "VoxelTile.h"


    struct VoxelTile {
    };

    DefineComponent(VoxelTile)

    EndComponent()

    DefineService(VoxelTile)

    EndService()

    static bool ServiceStart() {
        return true;
    }

    static bool ServiceStop() {
        return true;
    }
}