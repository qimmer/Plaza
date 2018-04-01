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

void SetShaderIncludeDirectory(StringRef value) {
    ShaderIncludeDirectory = value;
}

StringRef GetShaderIncludeDirectory() {
    return ShaderIncludeDirectory.c_str();
}

void CompileShader(Entity binaryShader) {
    FireEvent(ShaderCompile, binaryShader);
}

static void OnServiceStart(Service service) {
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    strcat(cwd, "/Shaders");

    ShaderIncludeDirectory = cwd;
}

DefineService(ShaderCompiler)
        Subscribe(ShaderCompilerStarted, OnServiceStart)
EndService()
