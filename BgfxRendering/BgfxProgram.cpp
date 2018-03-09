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

struct BgfxProgram {
    BgfxProgram() {
        for(auto i = 0; i < ShaderProfile_Max;++i) {
            handle[i] = BGFX_INVALID_HANDLE;
            invalidated[i] = true;
        }
    }
    bgfx::ProgramHandle handle[ShaderProfile_Max];
    bool invalidated[ShaderProfile_Max];
};

DefineComponent(BgfxProgram)
EndComponent()

DefineService(BgfxProgram)
EndService()

static bool IsVertexShader(Entity entity) {
    return HasShader(entity) && GetShaderType(entity) == ShaderType_Vertex;
}

static bool IsPixelShader(Entity entity) {
    return HasShader(entity) && GetShaderType(entity) == ShaderType_Pixel;
}

void OnProgramRemoved(Entity entity) {
    auto data = GetBgfxProgram(entity);

    for(auto i = 0; i < ShaderProfile_Max; ++i) {
        if(bgfx::isValid(data->handle[i])) {
            bgfx::destroy(data->handle[i]);
            data->handle[i] = BGFX_INVALID_HANDLE;
        }
    }
}

static void OnChanged(Entity entity) {
    if(HasBgfxProgram(entity)) {
        auto data = GetBgfxProgram(entity);
        for(auto i = 0; i < ShaderProfile_Max; ++i) {
            data->invalidated[i] = true;
        }
    }
}

static bool ServiceStart() {
    SubscribeBgfxProgramRemoved(OnProgramRemoved);
    SubscribeProgramChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeBgfxProgramRemoved(OnProgramRemoved);
    UnsubscribeProgramChanged(OnChanged);
    return true;
}

u16 GetBgfxProgramHandle(Entity entity, u8 shaderProfile) {
    auto data = GetBgfxProgram(entity);
    if(data->invalidated[shaderProfile]) {
        auto vertexShader = GetNextChildThat(entity, 0, IsVertexShader);
        auto pixelShader = GetNextChildThat(entity, 0, IsPixelShader);
        auto binaryVertexShader = GetBinaryShader(vertexShader, shaderProfile);
        auto binaryPixelShader = GetBinaryShader(pixelShader, shaderProfile);

        // Eventually free old buffers
        if(bgfx::isValid(data->handle[shaderProfile])) {
            bgfx::destroy(data->handle[shaderProfile]);
            data->handle[shaderProfile] = BGFX_INVALID_HANDLE;
        }

        auto vertexShaderHandle = GetBgfxBinaryShaderHandle(binaryVertexShader);
        auto pixelShaderHandle = GetBgfxBinaryShaderHandle(binaryPixelShader);

        if(vertexShaderHandle == bgfx::kInvalidHandle || pixelShaderHandle == bgfx::kInvalidHandle) return bgfx::kInvalidHandle;

        data->handle[shaderProfile] = bgfx::createProgram(bgfx::ShaderHandle {vertexShaderHandle}, bgfx::ShaderHandle {pixelShaderHandle});
        data->invalidated[shaderProfile] = false;
    }

    return data->handle[shaderProfile].idx;
}

