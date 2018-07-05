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

static void OnChanged(Entity entity) {
    if(HasBgfxBinaryShader(entity)) {
        GetBgfxBinaryShader(entity)->invalidated = true;
    }
}

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

        data->invalidated = false;

        if(size == 0) {
            Log(LogChannel_Core, LogSeverity_Error, "Binary shader data empty. Trying to compile ...");
            CompileShader(entity);

            return bgfx::kInvalidHandle;
        } else {
            auto buffer = malloc(size);
            StreamSeek(entity, 0);
            StreamRead(entity, size, buffer);
            StreamClose(entity);

            data->handle = bgfx::createShader(bgfx::copy(buffer, size));
            if(data->handle.idx == bgfx::kInvalidHandle) Log(LogChannel_Core, LogSeverity_Error, "Could not create binary shader handle.");
            free(buffer);
        }
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

DefineService(BgfxBinaryShader)
        Subscribe(BgfxBinaryShaderRemoved, OnBinaryShaderRemoved)
        Subscribe(BinaryShaderChanged, OnChanged)
        Subscribe(BgfxBinaryShaderChanged, OnChanged)
        Subscribe(StreamChanged, OnChanged)
        Subscribe(StreamContentChanged, OnChanged)
EndService()
