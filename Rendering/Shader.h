//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_SHADER_H
#define PLAZA_SHADER_H

#include <Core/Entity.h>


    enum {
        ShaderType_Unknown,
        ShaderType_Vertex,
        ShaderType_Pixel,
        ShaderType_Geometry,
        ShaderType_Hull,
        ShaderType_Domain,
        ShaderType_Compute
    };

    DeclareComponent(Shader)

    DeclareComponentProperty(Shader, Entity, ShaderSourceStream)
    DeclareComponentProperty(Shader, Entity, ShaderVaryingDefStream)
    DeclareComponentProperty(Shader, u8, ShaderType)

    DeclareService(Shader)

    Entity GetBinaryShader(Entity shader, StringRef profile);


#endif //PLAZA_SHADER_H
