//
// Created by Kim on 15-08-2018.
//

#include "ShaderCache.h"
#include <Rendering/Program.h>
#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include <Core/Hashing.h>

#include <EASTL/map.h>
#include <Core/Identification.h>
#include <Core/Enum.h>
#include <Core/Date.h>

using namespace eastl;

static Entity CreateBinaryProgram(Entity shaderCache, Entity program) {
    auto binaryProgram = CreateEntity();
    auto shaderCacheData = GetShaderCache(shaderCache);
    shaderCacheData.ShaderCachePrograms.Add(binaryProgram);
    SetShaderCache(shaderCache, shaderCacheData);

    auto data = GetBinaryProgram(binaryProgram);
    data.BinaryProgramProgram = program;
    SetBinaryProgram(binaryProgram, data);

    return binaryProgram;
}

API_EXPORT Entity GetShaderCacheBinaryProgram(
        Entity shaderCache,
        Entity program) {

    auto shaderCacheData = GetShaderCache(shaderCache);
    for(auto binaryProgram : shaderCacheData.ShaderCachePrograms) {
        auto binaryProgramData = GetBinaryProgram(binaryProgram);

        if(binaryProgramData.BinaryProgramProgram == program) {
            return binaryProgram;
        }
    }

    return CreateBinaryProgram(shaderCache, program);
}

static void InvalidateBinaryProgram(Entity binaryProgram) {
    auto binaryProgramData = GetBinaryProgram(binaryProgram);
    auto program = binaryProgramData.BinaryProgramProgram;
    auto programData = GetBinaryProgram(binaryProgram);
    auto shaderCache = GetOwnership(binaryProgram).Owner;

    auto cachePath = GetShaderCache(shaderCache).ShaderCacheDirectoryPath;
    if(!cachePath || cachePath[0] == '\0') {
        cachePath = ".shadercache";
    }

    auto binaryPath = StringFormatV("file://%s/%s_%s.vsb",
        cachePath,
        GetEnumName(EnumOf_ShaderProfile(), GetShaderCache(shaderCache).ShaderCacheProfile) + strlen("ShaderProfile_"),
        GetFileName(GetStream(GetProgram(program).ProgramVertexShaderSource).StreamPath));

    auto streamData = GetStream(binaryProgramData.BinaryProgramVertexShader);
    streamData.StreamPath = binaryPath;
    SetStream(binaryProgramData.BinaryProgramVertexShader, streamData);

    binaryPath = StringFormatV("file://%s/%s_%s.psb",
                               cachePath,
                               GetEnumName(EnumOf_ShaderProfile(), GetShaderCache(shaderCache).ShaderCacheProfile) + strlen("ShaderProfile_"),
                               GetFileName(GetStream(GetProgram(program).ProgramPixelShaderSource).StreamPath));

    streamData = GetStream(binaryProgramData.BinaryProgramPixelShader);
    streamData.StreamPath = binaryPath;
    SetStream(binaryProgramData.BinaryProgramPixelShader, streamData);

    SetBinaryProgram(binaryProgram, binaryProgramData);
}

static void OnStreamChanged(Entity stream, const Stream& oldData, const Stream& newData) {
    auto owner = GetOwnership(stream).Owner;
    if(HasComponent(owner, ComponentOf_Program())) {
        auto program = owner;

        BinaryProgram binaryProgramData;
        for_entity_data(binaryProgram, ComponentOf_BinaryProgram(), &binaryProgramData) {
            if(binaryProgramData.BinaryProgramProgram == program) {
                InvalidateBinaryProgram(binaryProgram);
            }
        }
    }
}

static void OnProgramChanged(Entity entity, const Program& oldProgram, const Program& newProgram) {
    BinaryProgram binaryProgramData;
    for_entity_data(binaryProgram, ComponentOf_BinaryProgram(), &binaryProgramData) {
        if(binaryProgramData.BinaryProgramProgram == entity) {
            InvalidateBinaryProgram(binaryProgram);
        }
    }
}

static void OnShaderCacheChanged(Entity shaderCache, const ShaderCache& oldData, const ShaderCache& newData) {
    for(auto binaryProgram : newData.ShaderCachePrograms) {
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
        BeginChildProperty(BinaryProgramVertexShader)
        EndChildProperty()
        BeginChildProperty(BinaryProgramPixelShader)
        EndChildProperty()
    EndComponent()

    BeginComponent(ShaderCache)
        RegisterProperty(StringRef, ShaderCacheDefines)
        RegisterProperty(u8, ShaderCacheProfile)
        RegisterProperty(StringRef, ShaderCacheDirectoryPath)
        RegisterArrayProperty(BinaryProgram, ShaderCachePrograms)
    EndComponent()

    RegisterDeferredSystem(OnStreamChanged, ComponentOf_Stream(), AppLoopOrder_Update + 1.0f)
    RegisterDeferredSystem(OnProgramChanged, ComponentOf_Program(), AppLoopOrder_Update + 1.0f)
    RegisterDeferredSystem(OnShaderCacheChanged, ComponentOf_ShaderCache(), AppLoopOrder_Update + 1.0f)
EndUnit()
