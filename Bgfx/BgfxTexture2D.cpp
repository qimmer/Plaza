//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxTexture2D.h"
#include <bgfx/bgfx.h>
#include <Foundation/Stream.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include <Foundation/Invalidation.h>


    struct BgfxTexture2D {
        BgfxTexture2D() :
                handle(BGFX_INVALID_HANDLE),
                size(0) {}

        bgfx::TextureHandle handle;
        u32 size;
    };

    DefineComponent(BgfxTexture2D)
    EndComponent()

    DefineService(BgfxTexture2D)
    EndService()

    void UpdateBgfxTexture2D(Entity entity) {
        if(!HasBgfxTexture2D(entity))
        {
            return;
        }

        auto data = GetBgfxTexture2D(entity);
        auto stream = GetTextureDataStream(entity);
        Assert(StreamOpen(stream));
        StreamSeek(stream, STREAM_SEEK_END);
        auto size = StreamTell(stream);

        if(size == 0) {
            StreamClose(stream);
            return;
        }

        auto buffer = malloc(size);
        StreamSeek(stream, 0);
        StreamRead(stream, size, buffer);
        StreamClose(stream);

        // Eventually free old buffers
        if((data->size != size || !GetTextureDynamic(entity)) || !bgfx::isValid(data->handle)) {
            if(bgfx::isValid(data->handle)) {
                bgfx::destroy(data->handle);
                data->handle = BGFX_INVALID_HANDLE;
            }

            auto dimensions = GetTextureSize2D(entity);
            data->handle = bgfx::createTexture2D(dimensions.x, dimensions.y, GetTextureMipLevels(entity), 1, (bgfx::TextureFormat::Enum)GetTextureFormat(entity), 0, bgfx::copy(buffer, size));
        } else {
            auto mipSize = GetTextureSize2D(entity);
            bgfx::updateTexture2D(data->handle, 0, 0, 0, 0, mipSize.x, mipSize.y, bgfx::copy(buffer, size));
        }

        free(buffer);
        data->size = size;
    }

    void OnTexture2DRemoved(Entity entity) {
        auto data = GetBgfxTexture2D(entity);

        if(bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }
    }

    static bool ServiceStart() {
        SubscribeBgfxTexture2DRemoved(OnTexture2DRemoved);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeBgfxTexture2DRemoved(OnTexture2DRemoved);
        return true;
    }

    u16 GetBgfxTexture2DHandle(Entity entity) {
        return GetBgfxTexture2D(entity)->handle.idx;
    }

