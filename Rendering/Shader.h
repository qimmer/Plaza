//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_SHADER_H
#define PLAZA_SHADER_H

#include <Core/Entity.h>

#define ShaderType_Unknown 0
#define ShaderType_Vertex 1
#define ShaderType_Pixel 2
#define ShaderType_Geometry 3
#define ShaderType_Hull 4
#define ShaderType_Domain 5
#define ShaderType_Compute 6

DeclareComponent(Shader)
DeclareComponentPropertyReactive(Shader, u8, ShaderType)

DeclareService(Shader)

Entity GetBinaryShader(Entity shader, u8 profile);

#endif //PLAZA_SHADER_H
