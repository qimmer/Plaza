//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxBinaryShader.h"
#include <Rendering/BinaryShader.h>
#include <bgfx/bgfx.h>
#include <Foundation/Stream.h>
#include <Rendering/ShaderCompiler.h>
#include <Rendering/Program.h>
#include <Rendering/Shader.h>

struct BgfxBinaryShader {
    BgfxBinaryShader() : handle(BGFX_INVALID_HANDLE), invalidated(true) {}

    bgfx::ShaderHandle handle;
    bool invalidated;
};

DefineComponent(BgfxBinaryShader)
EndComponent()

DefineService(BgfxBinaryShader)
EndService()

u16 GetBgfxBinaryShaderHandle(Entity entity) {
    auto data = GetBgfxBinaryShader(entity);
    if(data->invalidated) {
        // Eventually free old buffers
        if(bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }

        u32 size = 0;
        if(StreamOpen(entity, StreamMode_Read)) {
            StreamSeek(entity, StreamSeek_End);
            size = StreamTell(entity);
        }

        if(size == 0) {
            StreamClose(entity);
            CompileShader(GetSourceShader(entity), entity);
            return bgfx::kInvalidHandle;
        }

        auto buffer = malloc(size);
        StreamSeek(entity, 0);
        StreamRead(entity, size, buffer);
        StreamClose(entity);

        data->handle = bgfx::createShader(bgfx::copy(buffer, size));
        free(buffer);

        data->invalidated = false;
    }

    return data->handle.idx;
}

void OnBinaryShaderRemoved(Entity entity) {
    auto data = GetBgfxBinaryShader(entity);

    if(bgfx::isValid(data->handle)) {
        bgfx::destroy(data->handle);
        data->handle = BGFX_INVALID_HANDLE;
    }
}


static bool ServiceStart() {
    SubscribeBgfxBinaryShaderRemoved(OnBinaryShaderRemoved);
    return true;
}

static bool ServiceStop() {
    UnsubscribeBgfxBinaryShaderRemoved(OnBinaryShaderRemoved);
    return true;
}

