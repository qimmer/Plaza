//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Core/String.h>
#include "ShaderCompiler.h"
#include "Shader.h"
#include <unistd.h>
#include <Foundation/AppLoop.h>

DefineEvent(ShaderCompilerFinished, ShaderCompilerFinishHandler)
DefineEvent(ShaderCompile, ShaderCompileHandler)

String ShaderIncludeDirectory;

DefineService(ShaderCompiler)
    ServiceSetting(ShaderIncludeDirectory, SETTING_SHADER_INCLUDE_DIRECTORY)
EndService()

void SetShaderIncludeDirectory(StringRef value) {
    ShaderIncludeDirectory = value;
}

StringRef GetShaderIncludeDirectory() {
    return ShaderIncludeDirectory.c_str();
}

void CompileShader(Entity shader, Entity binaryShader) {
    FireEvent(ShaderCompile, shader, binaryShader);
}

static bool ServiceStart() {
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    strcat(cwd, "/Shaders");

    ShaderIncludeDirectory = cwd;

    return true;
}

static bool ServiceStop() {
    return true;
}
