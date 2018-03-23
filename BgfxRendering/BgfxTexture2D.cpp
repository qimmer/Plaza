//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxTexture2D.h"
#include "BgfxOffscreenRenderTarget.h"
#include <bgfx/bgfx.h>
#include <Foundation/Stream.h>
#include <Rendering/Texture.h>
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture2D.h>

struct BgfxTexture2D {
    BgfxTexture2D() :
            handle(BGFX_INVALID_HANDLE),
            size(0), flag(0), invalidated(true) {}

    bgfx::TextureHandle handle;
    u32 size, flag;
    bool invalidated;
};

DefineComponent(BgfxTexture2D)
EndComponent()

DefineService(BgfxTexture2D)
EndService()

static void OnChanged(Entity entity) {
    if(HasBgfxTexture2D(entity)) {
        GetBgfxTexture2D(entity)->invalidated = true;
    }
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
    SubscribeTextureChanged(OnChanged);
    SubscribeTexture2DChanged(OnChanged);
    SubscribeStreamChanged(OnChanged);
    SubscribeStreamContentChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeBgfxTexture2DRemoved(OnTexture2DRemoved);
    UnsubscribeTextureChanged(OnChanged);
    UnsubscribeTexture2DChanged(OnChanged);
    UnsubscribeStreamChanged(OnChanged);
    UnsubscribeStreamContentChanged(OnChanged);
    return true;
}

u16 GetBgfxTexture2DHandle(Entity entity) {
    if(!IsEntityValid(entity)) return bgfx::kInvalidHandle;

    auto data = GetBgfxTexture2D(entity);

    if(data->invalidated) {
        if(!StreamOpen(entity, StreamMode_Read)) return bgfx::kInvalidHandle;

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

            for_entity(renderTarget, BgfxOffscreenRenderTarget) {
                if(GetRenderTargetTexture0(renderTarget) == entity ||
                        GetRenderTargetTexture1(renderTarget) == entity ||
                        GetRenderTargetTexture2(renderTarget) == entity ||
                        GetRenderTargetTexture3(renderTarget) == entity ||
                        GetRenderTargetTexture4(renderTarget) == entity ||
                        GetRenderTargetTexture5(renderTarget) == entity ||
                        GetRenderTargetTexture6(renderTarget) == entity ||
                        GetRenderTargetTexture7(renderTarget) == entity) {
                    FireEvent(OffscreenRenderTargetChanged, renderTarget);
                    break;
                }
            }
        } else {
            bgfx::updateTexture2D(data->handle, 0, 0, 0, 0, dimensions.x, dimensions.y, bgfx::copy(buffer, info.storageSize));
        }

        free(buffer);
        data->size = info.storageSize;
        data->invalidated = false;
    }

    return data->handle.idx;
}

