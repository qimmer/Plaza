//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Core/Hierarchy.h>
#include "Material.h"


    struct Material {
        Entity MaterialProgram;
        u64 MaterialRenderState;
    };

    DefineComponent(Material)
        Dependency(Hierarchy)
    EndComponent()

    DefineComponentProperty(Material, Entity, MaterialProgram)
    DefineComponentProperty(Material, u64, MaterialRenderState)

