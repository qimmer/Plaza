//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Material.h"
#include "ShaderCache.h"
#include "Uniform.h"
#include <Core/Enum.h>

struct Material {
    Entity MaterialProgram;
};

BeginUnit(Material)
    BeginComponent(Material)
        RegisterProperty(Entity, MaterialProgram)
    EndComponent()
EndUnit()