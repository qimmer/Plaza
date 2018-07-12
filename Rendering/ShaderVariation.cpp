//
// Created by Kim Johannsen on 30-03-2018.
//

#include <Core/String.h>
#include "ShaderVariation.h"

struct ShaderVariation {
    String ShaderVariationDefines;
};

BeginUnit(ShaderVariation)
    BeginComponent(ShaderVariation)
    RegisterProperty(StringRef, ShaderVariationDefines)
EndComponent()

RegisterProperty(StringRef, ShaderVariationDefines)
