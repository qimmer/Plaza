//
// Created by Kim on 15-08-2018.
//

#include "ShaderCache.h"
#include <Rendering/Program.h>
#include <Foundation/Stream.h>
#include <Core/Hashing.h>

#include <EASTL/map.h>
#include <Core/Identification.h>
#include <Core/Enum.h>

using namespace eastl;

struct BinaryProgram {
    Entity BinaryProgramProgram;
    Entity BinaryProgramVertexShader;
    Entity BinaryProgramPixelShader;
};

struct ShaderCache {
    Vector(ShaderCachePrograms, Entity, 256)
    StringRef ShaderCacheDirectoryPath;
    StringRef ShaderCacheIncludePath;
    u8 ShaderCacheProfile;
    StringRef ShaderCacheDefines;
};

static Entity CreateBinaryProgram(Entity shaderCache, Entity program) {
    auto binaryProgram = AddShaderCachePrograms(shaderCache);
    SetBinaryProgramProgram(binaryProgram, program);

    return binaryProgram;
}

API_EXPORT Entity GetShaderCacheBinaryProgram(
        Entity shaderCache,
        Entity program) {

    for_children(binaryProgram, ShaderCachePrograms, shaderCache) {
        auto binaryProgramData = GetBinaryProgramData(binaryProgram);

        if(binaryProgramData->BinaryProgramProgram == program) {
            return binaryProgram;
        }
    }

    return CreateBinaryProgram(shaderCache, program);
}

static void InvalidateBinaryProgram(Entity binaryProgram) {
    char binaryPath[1024];

    auto program = GetBinaryProgramProgram(binaryProgram);
    auto shaderCache = GetOwner(binaryProgram);

    snprintf(binaryPath, sizeof(binaryPath), "%s/%s_%s.bin",
            GetShaderCacheDirectoryPath(shaderCache),
            GetEnumName(EnumOf_ShaderProfile(), GetShaderCacheProfile(shaderCache)),
            GetFileName(GetStreamPath(GetProgramVertexShaderSource(program))));
    SetStreamPath(GetBinaryProgramVertexShader(binaryProgram), binaryPath);

    snprintf(binaryPath, sizeof(binaryPath), "%s/%s_%s.bin",
             GetShaderCacheDirectoryPath(shaderCache),
             GetEnumName(EnumOf_ShaderProfile(), GetShaderCacheProfile(shaderCache)),
             GetFileName(GetStreamPath(GetProgramPixelShaderSource(program))));
    SetStreamPath(GetBinaryProgramPixelShader(binaryProgram), binaryPath);

    Type types[] = { TypeOf_Entity };
    const void *argumentPtrs[] = { &binaryProgram };

    FireEventFast(EventOf_ShaderCompile(), 1, types, argumentPtrs);
}

LocalFunction(OnStreamChanged, void,
        Entity stream) {
    auto owner = GetOwner(stream);
    if(HasComponent(owner, ComponentOf_Program())) {
        auto program = owner;

        for_entity(binaryProgram, binaryProgramData, BinaryProgram) {
            if(binaryProgramData->BinaryProgramProgram == program) {
                InvalidateBinaryProgram(binaryProgram);
            }
        }
    }
}

LocalFunction(OnProgramChanged, void,
              Entity program) {
    for_entity(binaryProgram, binaryProgramData, BinaryProgram) {
        if(binaryProgramData->BinaryProgramProgram == program) {
            InvalidateBinaryProgram(binaryProgram);
        }
    }
}

LocalFunction(OnShaderCacheChanged, void,
              Entity shaderCache) {
    for_children(binaryProgram, ShaderCachePrograms, shaderCache) {
        InvalidateBinaryProgram(binaryProgram);
    }
}

BeginUnit(ShaderCache)
    BeginEnum(ShaderProfile, false)
        RegisterFlag(ShaderProfile_Unknown)
        RegisterFlag(ShaderProfile_HLSL_3_0)
        RegisterFlag(ShaderProfile_HLSL_4_0)
        RegisterFlag(ShaderProfile_HLSL_5_0)
        RegisterFlag(ShaderProfile_GLSL_2_1)
        RegisterFlag(ShaderProfile_GLSL_3_2)
        RegisterFlag(ShaderProfile_GLSL_4_1)
        RegisterFlag(ShaderProfile_GLSL_4_3)
        RegisterFlag(ShaderProfile_Metal_OSX)
        RegisterFlag(ShaderProfile_Metal_iOS)
    EndEnum()

    BeginComponent(BinaryProgram)
        RegisterReferenceProperty(Program, BinaryProgramProgram)
        RegisterChildProperty(Stream, BinaryProgramVertexShader)
        RegisterChildProperty(Stream, BinaryProgramPixelShader)
    EndComponent()

    BeginComponent(ShaderCache)
        RegisterProperty(StringRef, ShaderCacheDefines)
        RegisterProperty(u8, ShaderCacheProfile)
        RegisterProperty(StringRef, ShaderCacheDirectoryPath)
        RegisterProperty(StringRef, ShaderCacheIncludePath)
        RegisterArrayProperty(BinaryProgram, ShaderCachePrograms)
    EndComponent()

    RegisterEvent(ShaderCompile)

    RegisterSubscription(StreamContentChanged, OnStreamChanged, 0)
    RegisterSubscription(StreamPathChanged, OnStreamChanged, 0)
    RegisterSubscription(ShaderCacheProfileChanged, OnShaderCacheChanged, 0)
    RegisterSubscription(ShaderCacheDefinesChanged, OnShaderCacheChanged, 0)
EndUnit()
