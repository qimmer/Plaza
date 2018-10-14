//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxTexture2D.h"
#include "BgfxOffscreenRenderTarget.h"
#include "BgfxResource.h"
#include <bgfx/bgfx.h>
#include <Foundation/Stream.h>
#include <Rendering/Texture.h>
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture2D.h>
#include <Foundation/Invalidation.h>

struct BgfxTexture2D {
    u32 size, flag;
};

LocalFunction(OnTexture2DRemoved, void, Entity entity) {
    bgfx::TextureHandle handle = { GetBgfxResourceHandle(entity) };

    if(bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }
}

LocalFunction(OnValidation, void, Entity component) {
    for_entity(entity, data, BgfxTexture2D) {
        bgfx::TextureHandle handle = { GetBgfxResourceHandle(entity) };
        auto data = GetBgfxTexture2DData(entity);

        if(!StreamOpen(entity, StreamMode_Read)) {
            return;
        }

        auto flag = GetTextureFlag(entity);
        auto dimensions = GetTextureSize2D(entity);
        auto format = (bgfx::TextureFormat::Enum)GetTextureFormat(entity);
        auto numMips = GetTextureMipLevels(entity);

        bgfx::TextureInfo info;
        bgfx::calcTextureSize(info, dimensions.x, dimensions.y, 1, false, numMips > 1, 1, format);

        auto buffer = malloc(info.storageSize);
        StreamDecompress(entity, 0, info.storageSize, buffer);

        // Eventually free old buffers
        if((data->flag != flag || data->size != info.storageSize || !GetTextureDynamic(entity)) || !bgfx::isValid(handle)) {
            OnTexture2DRemoved(entity);

            if(flag & TextureFlag_READ_BACK || flag & TextureFlag_RT) {
                SetBgfxResourceHandle(entity, bgfx::createTexture2D(dimensions.x, dimensions.y, GetTextureMipLevels(entity) > 1, 1, format, flag).idx);
            } else {
                SetBgfxResourceHandle(entity, bgfx::createTexture2D(dimensions.x, dimensions.y, GetTextureMipLevels(entity) > 1, 1, format, flag, bgfx::copy(buffer, info.storageSize)).idx);
            }
        } else {
            bgfx::updateTexture2D(handle, 0, 0, 0, 0, dimensions.x, dimensions.y, bgfx::copy(buffer, info.storageSize));
        }

        free(buffer);
        data->size = info.storageSize;
    }
}

BeginUnit(BgfxTexture2D)
    BeginComponent(BgfxTexture2D)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentRemoved(), OnTexture2DRemoved, ComponentOf_BgfxTexture2D())
    RegisterSubscription(EventOf_Validate(), OnValidation, ComponentOf_Texture())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureSize2D()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureFormat()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureFlag()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureDynamic()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureMipLevels()), Invalidate, 0)
EndUnit()
