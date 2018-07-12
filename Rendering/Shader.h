//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_SHADER_H
#define PLAZA_SHADER_H

#include <Core/NativeUtils.h>

DeclareEnum(ShaderType)

#define ShaderType_Unknown 0
#define ShaderType_Vertex 1
#define ShaderType_Pixel 2
#define ShaderType_Geometry 3
#define ShaderType_Hull 4
#define ShaderType_Domain 5
#define ShaderType_Compute 6

Unit(Shader)
    Component(Shader)
        Property(u8, ShaderType)
        Property(Entity, ShaderDeclaration)

#endif //PLAZA_SHADER_H
