//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxProgram.h"
#include "BgfxBinaryShader.h"
#include <bgfx/bgfx.h>
#include <Foundation/Invalidation.h>
#include <Rendering/Program.h>
#include <Rendering/Shader.h>


    struct BgfxProgram {
        BgfxProgram() : handle(BGFX_INVALID_HANDLE) {}
        bgfx::ProgramHandle handle;
    };

    DefineComponent(BgfxProgram)
    EndComponent()

    DefineService(BgfxProgram)
    EndService()

    void UpdateBgfxProgram(Entity entity) {
        if(!HasBgfxProgram(entity))
        {
            return;
        }

        auto data = GetBgfxProgram(entity);

        // Eventually free old buffers
        if(bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }

        auto profile = GetShadingProfile();
        auto vertexShader = GetVertexShader(entity);
        auto pixelShader = GetPixelShader(entity);
        auto binaryVertexShader = GetBinaryShader(vertexShader, profile);
        auto binaryPixelShader = GetBinaryShader(pixelShader, profile);

        data->handle = bgfx::createProgram(bgfx::ShaderHandle {GetBgfxBinaryShaderHandle(binaryVertexShader)}, bgfx::ShaderHandle {GetBgfxBinaryShaderHandle(binaryPixelShader)});
    }

    void OnProgramRemoved(Entity entity) {
        auto data = GetBgfxProgram(entity);

        if(bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }
    }

    static bool ServiceStart() {
        SubscribeBgfxProgramRemoved(OnProgramRemoved);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeBgfxProgramRemoved(OnProgramRemoved);
        return true;
    }

    u16 GetBgfxProgramHandle(Entity entity) {
        return GetBgfxProgram(entity)->handle.idx;
    }

