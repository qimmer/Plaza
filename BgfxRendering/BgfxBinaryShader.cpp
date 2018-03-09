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
            CompileShader(GetParent(entity), entity);
            return bgfx::kInvalidHandle;
        }

        auto buffer = malloc(size);
        StreamSeek(entity, 0);
        StreamRead(entity, size, buffer);
        StreamClose(entity);

        data->handle = bgfx::createShader(bgfx::copy(buffer, size));
        free(buffer);

        data->invalidated = false;

        FireEvent(ProgramChanged, GetParent(GetParent(entity)));
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

static void OnChanged(Entity entity) {
    if(HasBgfxBinaryShader(entity)) {
        GetBgfxBinaryShader(entity)->invalidated = true;

        auto shader = GetParent(entity);
        if(IsEntityValid(shader) && HasShader(shader)) {
            auto program = GetParent(shader);
            if(IsEntityValid(program) && HasProgram(program)) {
                FireEvent(ProgramChanged, program);
            }
        }
    }
}

static bool ServiceStart() {
    SubscribeBgfxBinaryShaderRemoved(OnBinaryShaderRemoved);
    SubscribeStreamChanged(OnChanged);
    SubscribeStreamContentChanged(OnChanged);
    SubscribeBinaryShaderChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeBgfxBinaryShaderRemoved(OnBinaryShaderRemoved);
    UnsubscribeStreamChanged(OnChanged);
    UnsubscribeStreamContentChanged(OnChanged);
    UnsubscribeBinaryShaderChanged(OnChanged);
    return true;
}

