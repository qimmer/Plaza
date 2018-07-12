//
// Created by Kim Johannsen on 16/01/2018.
//

#include <Core/String.h>
#include <Foundation/Stream.h>
#include "BinaryShader.h"

DefineEnum(ShaderProfile, false)
    DefineFlag(ShaderProfile_Unknown)
    DefineFlag(ShaderProfile_HLSL_3_0)
    DefineFlag(ShaderProfile_HLSL_4_0)
    DefineFlag(ShaderProfile_HLSL_5_0)
    DefineFlag(ShaderProfile_GLSL_2_1)
    DefineFlag(ShaderProfile_GLSL_3_2)
    DefineFlag(ShaderProfile_GLSL_4_1)
    DefineFlag(ShaderProfile_GLSL_4_3)
    DefineFlag(ShaderProfile_Metal_OSX)
    DefineFlag(ShaderProfile_Metal_iOS)
EndEnum()

struct BinaryShader {
    Entity SourceShader;
    u8 BinaryShaderProfile;
    Entity BinaryShaderVariation;
};

BeginUnit(BinaryShader)
    BeginComponent(BinaryShader)
    RegisterBase(Stream)
    DefinePropertyEnum(u8, BinaryShaderProfile, ShaderProfile)
    RegisterProperty(Entity, BinaryShaderVariation)
    RegisterProperty(Entity, SourceShader)
EndComponent()

RegisterProperty(Entity, SourceShader)
RegisterProperty(u8, BinaryShaderProfile)
RegisterProperty(Entity, BinaryShaderVariation)
