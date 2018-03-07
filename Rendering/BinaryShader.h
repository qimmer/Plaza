//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BINARYSHADER_H
#define PLAZA_BINARYSHADER_H

#include <Core/Entity.h>
#include <Core/Service.h>


    DeclareComponent(BinaryShader)

    DeclareComponentProperty(BinaryShader, Entity, SourceShader)
    DeclareComponentProperty(BinaryShader, StringRef, BinaryShaderProfile)

    DeclareService(BinaryShader)

    void SetSupportedShaderProfiles(StringRef profiles);
    StringRef GetSupportedShaderProfiles();

#define SETTING_SUPPORTED_SHADER_PROFILES "SupportedShaderProfiles"

#endif //PLAZA_BINARYSHADER_H
