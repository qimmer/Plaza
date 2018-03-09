//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BINARYSHADER_H
#define PLAZA_BINARYSHADER_H

#include <Core/Entity.h>
#include <Core/Service.h>

#define ShaderProfile_Unknown 0
#define ShaderProfile_HLSL_3_0 1
#define ShaderProfile_HLSL_4_0 2
#define ShaderProfile_HLSL_5_0 3
#define ShaderProfile_GLSL_2_1 4
#define ShaderProfile_GLSL_3_2 5
#define ShaderProfile_GLSL_4_1 6
#define ShaderProfile_GLSL_4_3 7
#define ShaderProfile_Metal_OSX 8
#define ShaderProfile_Metal_iOS 9
#define ShaderProfile_Max 16

DeclareComponent(BinaryShader)

DeclareComponentPropertyReactive(BinaryShader, Entity, SourceShader)
DeclareComponentPropertyReactive(BinaryShader, u8, BinaryShaderProfile)

#endif //PLAZA_BINARYSHADER_H
