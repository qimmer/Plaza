//
// Created by Kim Johannsen on 30-03-2018.
//

#include <Core/String.h>
#include "ShaderVariation.h"

struct ShaderVariation {
    String ShaderVariationDefines;
};

DefineComponent(ShaderVariation)
    DefinePropertyReactive(StringRef, ShaderVariationDefines)
EndComponent()

DefineComponentPropertyReactive(ShaderVariation, StringRef, ShaderVariationDefines)
