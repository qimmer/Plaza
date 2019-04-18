//
// Created by Kim on 21-08-2018.
//

#include <Foundation/Stream.h>
#include <Core/Debug.h>
#include "BgfxShaderCache.h"
#include "BgfxResource.h"
#include <Rendering/RenderingModule.h>

#include <bgfx/bgfx.h>
#include <Rendering/ShaderCache.h>
#include <Foundation/AppLoop.h>

#include <sstream>
#include <File/Folder.h>
#include <Rendering/Program.h>

struct BgfxBinaryProgram {
};


#ifdef _MSC_VER
#define popen _popen
#define pclose _pclose
#endif

static eastl::set<Entity> invalidatedBinaryPrograms;

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

    Info(0, "Compiling shader %s into %s using profile %s ...", sourcePath, binaryPath, profile);

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
    if(str.length()) {
        Log(0, severity, "%s", str.c_str());
    }

    return 0;
}

static void OnShaderCompile(Entity binaryProgram) {

    //ErrorRedirector errors(std::cerr);
    //ErrorRedirector output(std::cout);

    auto shaderCache = GetOwnership(binaryProgram).Owner;
    auto shaderCacheData = GetShaderCache(shaderCache);
    auto binaryProgramData = GetBinaryProgram(binaryProgram);
    auto programData = GetProgram(binaryProgramData.BinaryProgramProgram);

    auto shaderDefines = shaderCacheData.ShaderCacheDefines;
    auto profile = shaderCacheData.ShaderCacheProfile;
    auto binaryVertexShader = binaryProgramData.BinaryProgramVertexShader;
    auto binaryPixelShader = binaryProgramData.BinaryProgramPixelShader;

    auto vertexShader = programData.ProgramVertexShaderSource;
    auto pixelShader = programData.ProgramPixelShaderSource;
    auto declShader = programData.ProgramDeclShaderSource;

    bool hasErrors = false;

    // Vertex Shader
    hasErrors |= 0 != Compile(
            "file://BgfxRendering/Tools/shaderc.exe",
            GetStream(vertexShader).StreamResolvedPath,
            GetStream(declShader).StreamResolvedPath,
            GetStream(binaryVertexShader).StreamResolvedPath,
            "file://BgfxRendering/ShaderIncludes",
            0,
            profile,
            shaderDefines);

    // Pixel Shader
    hasErrors |= 0 != Compile(
            "file://BgfxRendering/Tools/shaderc.exe",
            GetStream(pixelShader).StreamResolvedPath,
            GetStream(declShader).StreamResolvedPath,
            GetStream(binaryPixelShader).StreamResolvedPath,
            "file://BgfxRendering/ShaderIncludes",
            1,
            profile,
            shaderDefines);
}

static void OnBgfxResourceChanged(Entity entity, const BgfxResource& oldData, const BgfxResource& newData) {
    if(oldData.BgfxResourceHandle != bgfx::kInvalidHandle && newData.BgfxResourceHandle != oldData.BgfxResourceHandle) {
        if(HasComponent(entity, ComponentOf_BgfxBinaryProgram())) {
            bgfx::ProgramHandle handle = { oldData.BgfxResourceHandle };

            if(bgfx::isValid(handle)) {
                bgfx::destroy(handle);
            }
        }
    }
}

static void free_func(void* mem, void *userdata) {
    _mm_free(mem);
}

static void LoadBinaryProgram(Entity entity) {
    auto binaryProgramData = GetBinaryProgram(entity);

    OnShaderCompile(entity);
    
    if(!StreamOpen(binaryProgramData.BinaryProgramVertexShader, StreamMode_Read)) {
        Log(binaryProgramData.BinaryProgramVertexShader, LogSeverity_Error, "Could not open binary vertex shader for reading.");
        return;
    }
    
    if(!StreamOpen(binaryProgramData.BinaryProgramPixelShader, StreamMode_Read)) {
        StreamClose(binaryProgramData.BinaryProgramVertexShader);
        Log(binaryProgramData.BinaryProgramPixelShader, LogSeverity_Error, "Could not open binary pixel shader for reading.");
        return;
    }

    StreamSeek(binaryProgramData.BinaryProgramVertexShader, StreamSeek_End);
    auto vsSize = StreamTell(binaryProgramData.BinaryProgramVertexShader);
    auto vs = _mm_malloc(vsSize, 16);
    StreamSeek(binaryProgramData.BinaryProgramVertexShader, 0);
    StreamRead(binaryProgramData.BinaryProgramVertexShader, vsSize, vs);
    StreamClose(binaryProgramData.BinaryProgramVertexShader);

    StreamSeek(binaryProgramData.BinaryProgramPixelShader, StreamSeek_End);
    auto psSize = StreamTell(binaryProgramData.BinaryProgramPixelShader);
    auto ps = _mm_malloc(psSize, 16);
    StreamSeek(binaryProgramData.BinaryProgramPixelShader, 0);
    StreamRead(binaryProgramData.BinaryProgramPixelShader, psSize, ps);
    StreamClose(binaryProgramData.BinaryProgramPixelShader);

    auto vsHandle = bgfx::createShader(bgfx::makeRef(vs, vsSize, free_func));
    auto psHandle = bgfx::createShader(bgfx::makeRef(ps, psSize, free_func));
    auto programHandle = bgfx::createProgram(vsHandle, psHandle, true);

    SetBgfxResource(entity, {programHandle.idx});
}

static void OnStreamChanged(Entity entity, const Stream& oldData, const Stream& newData) {
    auto owner = GetOwnership(entity).Owner;
    if(HasComponent(owner, ComponentOf_BgfxBinaryProgram())) {
        invalidatedBinaryPrograms.insert(owner);
    }
}

static void OnBinaryProgramChanged(Entity entity, const BinaryProgram& oldData, const BinaryProgram& newData) {
    if(HasComponent(entity, ComponentOf_BgfxBinaryProgram())) {
        invalidatedBinaryPrograms.insert(entity);
    }
}

static void OnAppLoopChanged(Entity entity, const AppLoop& oldData, const AppLoop& newData) {
    for(auto& entity : invalidatedBinaryPrograms) {
        LoadBinaryProgram(entity);
    }
    invalidatedBinaryPrograms.clear();
}

BeginUnit(BgfxShaderCache)
    BeginComponent(BgfxBinaryProgram)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSystem(OnStreamChanged, ComponentOf_Stream())
    RegisterSystem(OnBinaryProgramChanged, ComponentOf_BinaryProgram())
    RegisterSystem(OnBgfxResourceChanged, ComponentOf_BgfxResource())
    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_ResourceSubmission)
EndUnit()
