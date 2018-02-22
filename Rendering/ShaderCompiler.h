//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_SHADERCOMPILER_H
#define PLAZA_SHADERCOMPILER_H

#include <Core/Delegate.h>
#include <Core/Service.h>
#include <Core/Entity.h>

#define SETTING_SHADER_INCLUDE_DIRECTORY "shader_include_dir"


    typedef void(*ShaderCompilerFinishHandler)(bool hasErrors, StringRef output);

    DeclareEvent(ShaderCompilationNeeded, EntityHandler)
    DeclareEvent(ShaderCompilerFinished, ShaderCompilerFinishHandler)

    DeclareService(ShaderCompiler)

    void SetShaderIncludeDirectory(StringRef value);
    StringRef GetShaderIncludeDirectory();

#endif //PLAZA_SHADERCOMPILER_H
