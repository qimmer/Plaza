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

DefineComponent(BgfxProgram)
EndComponent()

static bool IsVertexShader(Entity entity) {
    return HasShader(entity) && GetShaderType(entity) == ShaderType_Vertex;
}

static bool IsPixelShader(Entity entity) {
    return HasShader(entity) && GetShaderType(entity) == ShaderType_Pixel;
}

void OnProgramRemoved(Entity entity) {
    auto data = GetBgfxProgram(entity);

    if (bgfx::isValid(data->handle)) {
        bgfx::destroy(data->handle);
        data->handle = BGFX_INVALID_HANDLE;
    }
}

static void OnChanged(Entity entity) {
    if(HasBgfxProgram(entity)) {
        auto data = GetBgfxProgram(entity);

        if (bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }
    }

    if(HasBinaryShader(entity)) {
        for_entity(program, BgfxProgram) {
            auto data = GetBgfxProgram(entity);

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
        Subscribe(BgfxProgramRemoved, OnProgramRemoved)
        Subscribe(ProgramChanged, OnChanged)
EndService()

u16 GetBgfxProgramHandle(Entity entity) {
    auto data = GetBgfxProgram(entity);

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

