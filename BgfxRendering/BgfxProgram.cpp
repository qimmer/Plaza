//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxProgram.h"
#include "BgfxBinaryShader.h"
#include <bgfx/bgfx.h>
#include <Rendering/Program.h>
#include <Rendering/Shader.h>
#include <Rendering/ShaderCompiler.h>
#include <Foundation/Stream.h>
#include <Rendering/BinaryShader.h>
#include <Rendering/ShaderVariation.h>

#define ShaderProfile_Max 32
#define ShaderVariation_Max 32

struct BgfxProgram {
    BgfxProgram() : handle(BGFX_INVALID_HANDLE) {}

    bgfx::ProgramHandle handle;
};

BeginUnit(BgfxProgram)
    BeginComponent(BgfxProgram)
EndComponent()

static bool IsVertexShader(Entity entity) {
    return HasComponent(entity, ComponentOf_Shader()) && GetShaderType(entity) == ShaderType_Vertex;
}

static bool IsPixelShader(Entity entity) {
    return HasComponent(entity, ComponentOf_Shader()) && GetShaderType(entity) == ShaderType_Pixel;
}

void OnProgramRemoved(Entity entity) {
    auto data = GetBgfxProgramData(entity);

    if (bgfx::isValid(data->handle)) {
        bgfx::destroy(data->handle);
        data->handle = BGFX_INVALID_HANDLE;
    }
}

LocalFunction(OnChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxProgram())) {
        auto data = GetBgfxProgramData(entity);

        if (bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }
    }

    if(HasComponent(entity, ComponentOf_BinaryShader())) {
        for_entity(program, data, BgfxProgram) {
            auto data = GetBgfxProgramData(entity);

            if(GetProgramBinaryVertexShader(program) == entity ||
                GetProgramBinaryPixelShader(program) == entity) {
                if (bgfx::isValid(data->handle)) {
                    bgfx::destroy(data->handle);
                    data->handle = BGFX_INVALID_HANDLE;
                }
            }
        }
    }
}

DefineService(BgfxProgram)
        RegisterSubscription(BgfxProgramRemoved, OnProgramRemoved, 0)
        RegisterSubscription(ProgramChanged, OnChanged, 0)
EndService()

u16 GetBgfxProgramHandle(Entity entity) {
    auto data = GetBgfxProgramData(entity);

    if(data->handle.idx == bgfx::kInvalidHandle) {
        auto binaryVertexShader = GetProgramBinaryVertexShader(entity);
        auto binaryPixelShader = GetProgramBinaryPixelShader(entity);

        if(!IsEntityValid(binaryVertexShader) || !IsEntityValid(binaryPixelShader)) {
            return bgfx::kInvalidHandle;
        }

        auto vertexShaderHandle = GetBgfxBinaryShaderHandle(binaryVertexShader);
        auto pixelShaderHandle = GetBgfxBinaryShaderHandle(binaryPixelShader);

        if(vertexShaderHandle == bgfx::kInvalidHandle || pixelShaderHandle == bgfx::kInvalidHandle) {
            return bgfx::kInvalidHandle;
        }

        data->handle = bgfx::createProgram(bgfx::ShaderHandle {vertexShaderHandle}, bgfx::ShaderHandle {pixelShaderHandle});

        if(data->handle.idx == bgfx::kInvalidHandle) {
            Log(LogChannel_Core, LogSeverity_Error, "Could not create program handle. Check that vertex shader and pixel shader varyings are matching.");
        }
    }

    return data->handle.idx;
}

