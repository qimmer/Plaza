//
// Created by Kim Johannsen on 17/01/2018.
//

#ifndef PLAZA_SHADERCOMPILER_H
#define PLAZA_SHADERCOMPILER_H

#include <Core/Event.h>
#include <Core/Entity.h>

#define SETTING_SHADER_INCLUDE_DIRECTORY "shader_include_dir"
#define LogChannel_ShaderCompiler 359

Event(ShaderCompile)
Event(ShaderCompilerFinished, bool hasErrors, StringRef output)


void SetShaderIncludeDirectory(StringRef value);
StringRef GetShaderIncludeDirectory();

void CompileShader(Entity binaryShader);

#endif //PLAZA_SHADERCOMPILER_H
