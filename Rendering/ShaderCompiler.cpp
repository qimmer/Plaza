//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Core/String.h>
#include "ShaderCompiler.h"
#include "Shader.h"
#include <unistd.h>
#include <Foundation/AppLoop.h>

RegisterEvent(ShaderCompilerFinished)
RegisterEvent(ShaderCompile)

String ShaderIncludeDirectory;

API_EXPORT void SetShaderIncludeDirectory(StringRef value) {
    ShaderIncludeDirectory = value;
}

API_EXPORT StringRef GetShaderIncludeDirectory() {
    return ShaderIncludeDirectory.c_str();
}

API_EXPORT void CompileShader(Entity binaryShader) {
    FireEvent(EventOf_ShaderCompile(), binaryShader);
}

LocalFunction(OnServiceStart, void, Service service) {
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    strcat(cwd, "/Shaders");

    ShaderIncludeDirectory = cwd;
}

DefineService(ShaderCompiler)
        RegisterSubscription(ShaderCompilerStarted, OnServiceStart, 0)
EndService()
