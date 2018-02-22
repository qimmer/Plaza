//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxBinaryShader.h"
#include <Rendering/BinaryShader.h>
#include <bgfx/bgfx.h>
#include <Foundation/Stream.h>
#include <Foundation/Invalidation.h>


    struct BgfxBinaryShader {
        BgfxBinaryShader() : handle(BGFX_INVALID_HANDLE) {}

        bgfx::ShaderHandle handle;
    };

    DefineComponent(BgfxBinaryShader)
    EndComponent()

    DefineService(BgfxBinaryShader)
    EndService()

    u16 GetBgfxBinaryShaderHandle(Entity entity) {
        return GetBgfxBinaryShader(entity)->handle.idx;
    }

    void UpdateBgfxBinaryShader(Entity entity) {
        if(!HasBgfxBinaryShader(entity))
        {
            return;
        }

        auto data = GetBgfxBinaryShader(entity);

        // Eventually free old buffers
        if(bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }

        auto stream = GetBinaryShaderStream(entity);
        Assert(StreamOpen(stream));
        StreamSeek(stream, STREAM_SEEK_END);
        auto size = StreamTell(stream);

        if(size == 0) {
            StreamClose(stream);
            return;
        }

        auto buffer = alloca(size);
        StreamSeek(stream, 0);
        StreamRead(stream, size, buffer);
        StreamClose(stream);

        data->handle = bgfx::createShader(bgfx::copy(buffer, size));
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

