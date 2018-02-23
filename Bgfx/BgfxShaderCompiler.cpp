//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Foundation/Invalidation.h>
#include <Rendering/Shader.h>
#include <Rendering/BinaryShader.h>
#include "BgfxShaderCompiler.h"
#include <bx/process.h>
#include <Core/String.h>
#include <iostream>
#include <sstream>
#include <File/FileStream.h>
#include <Rendering/ShaderCompiler.h>
#include "shaderc.h"

#include <sstream>
#include <string>
#include <iterator>
#include <climits>
#include <Core/Hierarchy.h>


static const StringRef shaderTypes[] = {
            "",
            "vertex",
            "fragment",
            "geometry",
            "hull",
            "domain",
            "compute"
    };

    struct ErrorRedirector {
        ErrorRedirector(std::ostream& s) : systemStream(s)
        {
            old = s.rdbuf( stream.rdbuf() );
        }

        void close() {
            systemStream.rdbuf( old );
        }

        ~ErrorRedirector( ) {
            close();
        }

        std::string capture() {
            return stream.str();
        }

    private:
        std::ostream& systemStream;
        std::streambuf *old;
        std::stringstream stream;
    };

    DefineService(BgfxShaderCompiler)
        ServiceDependency(ShaderCompiler)
    EndService()

    int Compile(StringRef sourceFilePath,
                 StringRef varyingDefFilePath,
                 StringRef outputFilePath,
                 StringRef platformProfile,
                 u8 shaderType) {
        if(!strlen(sourceFilePath) ||
                !strlen(varyingDefFilePath) ||
                !strlen(outputFilePath) ||
                !strlen(platformProfile)) {
            return -1;
        }



        char platformProfileSplit[PATH_MAX];
        strncpy(platformProfileSplit, platformProfile, PATH_MAX);
         
        u32 firstUnderscoreOffset = strchr(platformProfileSplit, '_') - platformProfileSplit;
        platformProfileSplit[firstUnderscoreOffset] = '\0';
        const char *platform = platformProfileSplit;
        const char *profile = platformProfileSplit + firstUnderscoreOffset + 1;

        StringRef arguments[] = {
            "-f", sourceFilePath,
            "-o", outputFilePath,
            "-p", profile,
            "-i", GetShaderIncludeDirectory(),
            "--type", shaderTypes[shaderType],
            "--platform", platform,
            "--varyingdef", varyingDefFilePath,
            "-O", "3"
        };

        auto result = bgfx::compileShader(16, arguments);
        return result;
    }

    void OnCompile(Entity shaderEntity) {
        if(!HasShader(shaderEntity)) return;

        auto shaderSourceStream = GetShaderSourceStream(shaderEntity);
        auto varyingDefStream = GetShaderVaryingDefStream(shaderEntity);

        if(!IsEntityValid(shaderSourceStream) ||
           !IsEntityValid(varyingDefStream) ||
            !IsEntityValid(GetShaderSourceStream(shaderEntity))) {
            return;
        }

        //ErrorRedirector errors(std::cerr);
        //ErrorRedirector output(std::cout);

        String profiles = GetSupportedShaderProfiles();
        if(profiles[profiles.length() - 1] != ';') {
            profiles += ';';
        }

        std::stringstream ss(profiles);
        String profile;
        bool hasErrors = false;

        while (std::getline(ss, profile, ';')) {
            auto binaryShader = GetBinaryShader(shaderEntity, profile.c_str());

            char binaryPath[PATH_MAX];
            sprintf(binaryPath, "Shaders/%zu_%s.bin", std::hash<std::string>()(GetEntityPath(shaderEntity)), profile.c_str());

            SetFilePath(GetBinaryShaderStream(binaryShader), binaryPath);

            Log(LogChannel_Core, LogSeverity_Info, "Compiling %s shader '%s' to '%s' with profile '%s' ...",
                shaderTypes[GetShaderType(shaderEntity)],
                GetFilePath(shaderSourceStream),
                GetFilePath(GetBinaryShaderStream(binaryShader)),
                profile.c_str());

            auto binaryShaderStream = GetBinaryShaderStream(binaryShader);

            hasErrors |= Compile(
                    GetFilePath(shaderSourceStream),
                    GetFilePath(varyingDefStream),
                    GetFilePath(binaryShaderStream),
                    profile.c_str(),
                    GetShaderType(shaderEntity));

            if(IsEntityValid(binaryShaderStream)) {
                SetInvalidated(binaryShaderStream, true);
            }
        }

        FireEvent(ShaderCompilerFinished, hasErrors, "");
    }


    static bool ServiceStart() {
        SubscribeShaderCompilationNeeded(OnCompile);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeShaderCompilationNeeded(OnCompile);
        return true;
    }
