//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Core/Hierarchy.h>
#include "Material.h"


    struct Material {
        Material() : MaterialRenderState(RenderState_STATE_DEFAULT) {}

        Entity MaterialProgram;
        u64 MaterialRenderState;
    };

    DefineComponent(Material)
        Dependency(Hierarchy)
        DefineProperty(Entity, MaterialProgram)
        DefineProperty(u64, MaterialRenderState)
    EndComponent()

    DefineComponentProperty(Material, Entity, MaterialProgram)
    DefineComponentProperty(Material, u64, MaterialRenderState)

