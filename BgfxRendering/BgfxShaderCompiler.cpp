//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Foundation/Invalidation.h>
#include <Rendering/Shader.h>
#include <Rendering/BinaryShader.h>
#include "BgfxShaderCompiler.h"
#include <Core/String.h>
#include <iostream>
#include <sstream>
#include <Rendering/ShaderCompiler.h>

#include <sstream>
#include <string>
#include <iterator>
#include <climits>
#include <Core/Hierarchy.h>
#include <Rendering/Program.h>
#include <Foundation/Stream.h>
#include <File/Folder.h>


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

        if(memcmp(sourceFilePath, "file://", 7) != 0 ||
                memcmp(varyingDefFilePath, "file://", 7) != 0 ||
                memcmp(outputFilePath, "file://", 7) != 0) {
            Log(LogChannel_ShaderCompiler, LogSeverity_Error, "Bgfx shader compiler tool only supports file:// paths.");
            return -1;
        }

        CreateDirectories(GetParentFolder(outputFilePath));

        sourceFilePath += 7;
        varyingDefFilePath += 7;
        outputFilePath += 7;

        char platformProfileSplit[PATH_MAX];
        strncpy(platformProfileSplit, platformProfile, PATH_MAX);
         
        u32 firstUnderscoreOffset = strchr(platformProfileSplit, '_') - platformProfileSplit;
        platformProfileSplit[firstUnderscoreOffset] = '\0';
        char *platform = platformProfileSplit;
        char *profile = platformProfileSplit + firstUnderscoreOffset + 1;

        if(profile[0] == 'x') {
            switch(shaderType) {
                case ShaderType_Vertex:
                    profile[0] = 'v';
                    break;
                case ShaderType_Pixel:
                    profile[0] = 'p';
                    break;
                case ShaderType_Geometry:
                    profile[0] = 'g';
                    break;
                case ShaderType_Hull:
                    profile[0] = 'h';
                    break;
                case ShaderType_Domain:
                    profile[0] = 'd';
                    break;
            }
        }

        static std::stringstream ss;
        ss.clear();
        FILE *fpipe;
        char c = 0;
        auto command = FormatString("./shaderc -f \"%s\" -o \"%s\" -p %s -i \"%s\" --type %s --platform %s --varyingdef \"%s\" -O3",
                                    sourceFilePath,
                                    outputFilePath,
                                    profile,
                                    GetShaderIncludeDirectory(),
                                    shaderTypes[shaderType],
                                    platform,
                                    varyingDefFilePath);

        if (0 == (fpipe = (FILE*)popen(command, "r")))
        {
            Log(LogChannel_Core, LogSeverity_Error, "Bgfx Shader Compiler 'shaderc' not found.");
            return -1;
        }

        while (fread(&c, sizeof c, 1, fpipe))
        {
            if(c) {
                ss << c;
            }
        }


        auto severity = LogSeverity_Info;
        if(pclose(fpipe))
        {
            severity = LogSeverity_Error;
        }

        auto str = ss.str();

        Log(LogChannel_ShaderCompiler, severity, "%s", str.c_str());
    }

    void OnCompile(Entity shaderEntity) {
        if(!HasShader(shaderEntity) || !HasProgram(GetParent(shaderEntity))) return;

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
            sprintf(binaryPath, "res://shadercache/%zu_%s.bin", std::hash<std::string>()(GetEntityPath(shaderEntity)), profile.c_str());

            SetStreamPath(binaryShader, binaryPath);

            Log(LogChannel_Core, LogSeverity_Info, "Compiling %s shader '%s' to '%s' with profile '%s' using program (varying def) '%s' ...",
                shaderTypes[GetShaderType(shaderEntity)],
                GetStreamResolvedPath(shaderEntity),
                GetStreamResolvedPath(binaryShader),
                profile.c_str(),
                GetStreamResolvedPath(GetParent(shaderEntity)));

            hasErrors |= Compile(
                    GetStreamResolvedPath(shaderEntity),
                    GetStreamResolvedPath(GetParent(shaderEntity)),
                    GetStreamResolvedPath(binaryShader),
                    profile.c_str(),
                    GetShaderType(shaderEntity));

            if(IsEntityValid(binaryShader)) {
                SetInvalidated(binaryShader, true);
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
