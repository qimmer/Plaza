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
        u32 size, flag;
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
        if(!StreamOpen(entity, StreamMode_Read)) return;

        auto flag = GetTextureFlag(entity);
        auto dimensions = GetTextureSize2D(entity);
        auto format = (bgfx::TextureFormat::Enum)GetTextureFormat(entity);
        auto numMips = GetTextureMipLevels(entity);

        bgfx::TextureInfo info;
        bgfx::calcTextureSize(info, dimensions.x, dimensions.y, 1, false, numMips > 1, 1, format);

        auto buffer = malloc(info.storageSize);
        StreamDecompress(entity, 0, info.storageSize, buffer);

        // Eventually free old buffers
        if((data->flag != flag || data->size != info.storageSize || !GetTextureDynamic(entity)) || !bgfx::isValid(data->handle)) {
            if(bgfx::isValid(data->handle)) {
                bgfx::destroy(data->handle);
                data->handle = BGFX_INVALID_HANDLE;
            }


            data->handle = bgfx::createTexture2D(dimensions.x, dimensions.y, GetTextureMipLevels(entity) > 1, 1, format, flag, bgfx::copy(buffer, info.storageSize));
        } else {
            bgfx::updateTexture2D(data->handle, 0, 0, 0, 0, dimensions.x, dimensions.y, bgfx::copy(buffer, info.storageSize));
        }

        free(buffer);
        data->size = info.storageSize;
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

