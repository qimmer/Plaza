//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Core/String.h>
#include "ShaderCompiler.h"
#include "Shader.h"
#include <unistd.h>
#include <Foundation/Invalidation.h>
#include <Foundation/AppLoop.h>


    DefineEvent(ShaderCompilerFinished, ShaderCompilerFinishHandler)
    DefineEvent(ShaderCompilationNeeded, EntityHandler)

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

    static void CompileIfNeeded(Entity shader) {
        FireEvent(ShaderCompilationNeeded, shader);
    }

    static void OnAppUpdate(double deltaTime) {
        ValidateAll(CompileIfNeeded, HasShader);
    }

    static bool ServiceStart() {
        char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        strcat(cwd, "/Shaders");

        ShaderIncludeDirectory = cwd;

        SubscribeAppUpdate(OnAppUpdate);
        
        return true;
    }

    static bool ServiceStop() {

        UnsubscribeAppUpdate(OnAppUpdate);

        return true;
    }
