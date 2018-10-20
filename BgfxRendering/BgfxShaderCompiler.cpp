//
// Created by Kim Johannsen on 17/01/2018.
//

#include "BgfxShaderCompiler.h"
#include <Rendering/Program.h>
#include <Foundation/Stream.h>
#include <File/Folder.h>
#include <Core/Base64.h>
#include <Core/Debug.h>

#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <climits>
#include <Core/Identification.h>
#include <Rendering/ShaderCache.h>
#include <Foundation/Invalidation.h>

static const StringRef shaderTypes[] = {
    "vertex",
    "fragment",
    "geometry",
    "hull",
    "domain",
    "compute"
};

static const char shaderTypeProfiles[] = {
        'v',
        'p',
        'g',
        'h',
        'd',
        'c'
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

int Compile(StringRef compilerPath,
            StringRef sourcePath,
            StringRef declSourcePath,
            StringRef binaryPath,
            StringRef includePath,
            u8 shaderType,
            u8 shaderProfile,
            StringRef defines) {
    StringRef *paths[] = {
        &compilerPath,
        &sourcePath,
        &declSourcePath,
        &binaryPath,
        &includePath
    };

    auto numPaths = sizeof(paths) / sizeof(StringRef);

    for(auto i = 0; i < numPaths; ++i) {
        if(!*paths[i] || !strlen(*paths[i]) || memcmp(*paths[i], "file://", 7) != 0) {
            Log(0, LogSeverity_Error, "Bgfx shader compiler tool only supports file:// paths.");
            return -1;
        }

        *paths[i] += 7; // Skip 'file://'
    }

    auto parentPath = GetParentFolder(binaryPath);
    CreateDirectories(parentPath);

    char profile[128];
    const char *platform;
    switch(shaderProfile) {
        case ShaderProfile_HLSL_3_0:
            sprintf(profile, "%c%s", shaderTypeProfiles[shaderType], "s_3_0");
            platform = "windows";
            break;
        case ShaderProfile_HLSL_4_0:
            sprintf(profile, "%c%s", shaderTypeProfiles[shaderType], "s_4_0");
            platform = "windows";
            break;
        case ShaderProfile_HLSL_5_0:
            sprintf(profile, "%c%s", shaderTypeProfiles[shaderType], "s_5_0");
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

    static std::stringstream ss;
    ss.clear();
    FILE *fpipe;
    char c = 0;

    auto hasDefines = defines != 0 && strlen(defines) > 0;

    char command[4096];
    snprintf(command,
             4096,
             "%s -f \"%s\" -o \"%s\" -p %s -i \"%s\" --type %s --platform %s --varyingdef \"%s\" -O3 %s %s",
             compilerPath,
            sourcePath,
            binaryPath,
            profile,
            includePath,
            shaderTypes[shaderType],
            platform,
            declSourcePath,
             hasDefines ? "--define" : "",
             hasDefines ? defines : "");

#ifdef WIN32
    // Convert slashes in paths into backslashes
    char *ptr = command;
    while(*ptr) {
        if(*ptr == '/') *ptr = '\\';
        ptr++;
    }
#endif

    if (0 == (fpipe = (FILE*)popen(command, "r")))
    {
        Log(0, LogSeverity_Error, "Bgfx Shader Compiler 'shaderc' not found.");
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

    Log(0, severity, "%s", str.c_str());
	
	return 0;
}

LocalFunction(OnShaderCompile, void, Entity binaryProgram) {

    //ErrorRedirector errors(std::cerr);
    //ErrorRedirector output(std::cout);

    auto shaderCache = GetOwner(binaryProgram);
    auto shaderDefines = GetShaderCacheDefines(shaderCache);
    auto profile = GetShaderCacheProfile(shaderCache);

    auto binaryVertexShader = GetBinaryProgramVertexShader(binaryProgram);
    auto binaryPixelShader = GetBinaryProgramPixelShader(binaryProgram);

    auto program = GetBinaryProgramProgram(binaryProgram);
    auto vertexShader = GetProgramVertexShaderSource(program);
    auto pixelShader = GetProgramPixelShaderSource(program);
    auto declShader = GetProgramDeclShaderSource(program);

    bool hasErrors = false;

    // Vertex Shader
    hasErrors |= 0 != Compile(
            "file://BgfxRendering/Tools/shaderc.exe",
            GetStreamResolvedPath(vertexShader),
            GetStreamResolvedPath(declShader),
            GetStreamResolvedPath(binaryVertexShader),
            "file://BgfxRendering/ShaderIncludes",
            0,
            profile,
            shaderDefines);

    // Pixel Shader
    hasErrors |= 0 != Compile(
            "file://BgfxRendering/Tools/shaderc.exe",
            GetStreamResolvedPath(pixelShader),
            GetStreamResolvedPath(declShader),
            GetStreamResolvedPath(binaryPixelShader),
            "file://BgfxRendering/ShaderIncludes",
            1,
            profile,
            shaderDefines);

    Invalidate(binaryProgram);
}

BeginUnit(BgfxShaderCompiler)
    RegisterSubscription(EventOf_ShaderCompile(), OnShaderCompile, 0)
EndUnit()