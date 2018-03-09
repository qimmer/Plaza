//
// Created by Kim Johannsen on 17/01/2018.
//

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
#include <Core/Base64.h>

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

void GetBinaryShaderFilePath(Entity binaryShader, char *out) {
    u64 hash = std::hash<std::string>()(GetEntityPath(binaryShader));
    u8 bytes[9];
    memcpy(bytes, &hash, sizeof(u64));
    bytes[8] = GetBinaryShaderProfile(binaryShader);

    s8 encodedFileName[128];
    Base64Encode(bytes, 9, encodedFileName);

    sprintf(out, "res://shadercache/%s.bin", encodedFileName);
}

int Compile(StringRef sourceFilePath,
             StringRef varyingDefFilePath,
             StringRef outputFilePath,
             u8 shaderProfile,
             u8 shaderType) {
    if(!strlen(sourceFilePath) ||
            !strlen(varyingDefFilePath) ||
            !strlen(outputFilePath) ||
            !shaderProfile) {
        return -1;
    }

    if(memcmp(sourceFilePath, "file://", 7) != 0 ||
            memcmp(varyingDefFilePath, "file://", 7) != 0 ||
            memcmp(outputFilePath, "file://", 7) != 0) {
        Log(LogChannel_ShaderCompiler, LogSeverity_Error, "Bgfx shader compiler tool only supports file:// paths.");
        return -1;
    }

    char parentPath[PATH_MAX];
    GetParentFolder(outputFilePath, parentPath);
    CreateDirectories(parentPath);

    sourceFilePath += 7;
    varyingDefFilePath += 7;
    outputFilePath += 7;

    char profile[128];
    const char *platform;
    switch(shaderProfile) {
        case ShaderProfile_HLSL_3_0:
            sprintf(profile, "%s", "xs_3_0");
            platform = "windows";
            break;
        case ShaderProfile_HLSL_4_0:
            sprintf(profile, "%s", "xs_4_0");
            platform = "windows";
            break;
        case ShaderProfile_HLSL_5_0:
            sprintf(profile, "%s", "xs_5_0");
            platform = "windows";
            break;
        case ShaderProfile_GLSL_2_1:
            sprintf(profile, "%s", "120");
            platform = "linux";
            break;
        case ShaderProfile_GLSL_3_2:
            sprintf(profile, "%s", "320");
            platform = "linux";
            break;
        case ShaderProfile_GLSL_4_1:
            sprintf(profile, "%s", "410");
            platform = "linux";
            break;
        case ShaderProfile_GLSL_4_3:
            sprintf(profile, "%s", "430");
            platform = "linux";
            break;
        case ShaderProfile_Metal_OSX:
            sprintf(profile, "%s", "metal");
            platform = "osx";
            break;
        default:
            return -1;
    }

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

    char command[4096];
    snprintf(command,
             4096,
             "shaderc -f \"%s\" -o \"%s\" -p %s -i \"%s\" --type %s --platform %s --varyingdef \"%s\" -O3",
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

void OnCompile(Entity shader, Entity binaryShader) {

    //ErrorRedirector errors(std::cerr);
    //ErrorRedirector output(std::cout);

    auto profile = GetBinaryShaderProfile(binaryShader);

    char binaryPath[PATH_MAX];
    GetBinaryShaderFilePath(binaryShader, binaryPath);

    SetStreamPath(binaryShader, binaryPath);

    Log(LogChannel_Core, LogSeverity_Info, "Compiling %s shader '%s' to '%s' with profile '%d' using program (varying def) '%s' ...",
        shaderTypes[GetShaderType(shader)],
        GetStreamResolvedPath(shader),
        GetStreamResolvedPath(binaryShader),
        profile,
        GetStreamResolvedPath(GetParent(shader)));

    bool hasErrors = Compile(
            GetStreamResolvedPath(shader),
            GetStreamResolvedPath(GetParent(shader)),
            GetStreamResolvedPath(binaryShader),
            profile,
            GetShaderType(shader));

    FireEvent(ShaderCompilerFinished, hasErrors, "");
}


static bool ServiceStart() {
    SubscribeShaderCompile(OnCompile);
    return true;
}

static bool ServiceStop() {
    UnsubscribeShaderCompile(OnCompile);
    return true;
}
