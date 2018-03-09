//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/String.h>
#include <Foundation/Stream.h>
#include "BinaryShader.h"

struct BinaryShader {
    Entity SourceShader;
    u8 BinaryShaderProfile;
};

DefineComponent(BinaryShader)
    Dependency(Stream)
    DefineProperty(Entity, BinaryShaderProfile)
EndComponent()

DefineComponentPropertyReactive(BinaryShader, Entity, SourceShader)
DefineComponentPropertyReactive(BinaryShader, u8, BinaryShaderProfile)
