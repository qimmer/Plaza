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
#include <Rendering/RenderContext.h>
#include <Rendering/RenderingModule.h>
#include <Foundation/AppLoop.h>

struct BgfxTexture2D {
    u32 size, flag;
};

static eastl::set<Entity> invalidatedTextures;

static void OnBgfxResourceChanged(Entity entity, const BgfxResource& oldData, const BgfxResource& newData) {
    if(oldData.BgfxResourceHandle != bgfx::kInvalidHandle && newData.BgfxResourceHandle != oldData.BgfxResourceHandle) {
        if(HasComponent(entity, ComponentOf_BgfxOffscreenRenderTarget())) {
            bgfx::FrameBufferHandle handle = { oldData.BgfxResourceHandle };

            if(bgfx::isValid(handle)) {
                bgfx::destroy(handle);
            }
        }
    }
}

static void ValidateTexture(Entity entity) {
    auto data = GetBgfxTexture2D(entity);
    auto textureData = GetTexture(entity);
    auto texture2DData = GetTexture2D(entity);

    bgfx::TextureHandle handle = { GetBgfxResource(entity).BgfxResourceHandle };

    if(!StreamOpen(entity, StreamMode_Read)) {
        return;
    }

    auto flag = textureData.TextureFlag;
    auto dimensions = texture2DData.TextureSize2D;
    auto format = (bgfx::TextureFormat::Enum)textureData.TextureFormat;
    auto numMips = textureData.TextureMipLevels;

    bgfx::TextureInfo info;
    bgfx::calcTextureSize(info, dimensions.x, dimensions.y, 1, false, numMips > 1, 1, format);

    auto buffer = malloc(info.storageSize);
    StreamDecompress(entity, 0, info.storageSize, buffer);

    // Eventually free old buffers
    if((data.flag != flag || data.size != info.storageSize || !textureData.TextureDynamic) || !bgfx::isValid(handle)) {
        auto textureHandle = (flag & TextureFlag_READ_BACK || flag & TextureFlag_RT)
            ? bgfx::createTexture2D(dimensions.x, dimensions.y, textureData.TextureMipLevels > 1, 1, format, flag).idx
            : bgfx::createTexture2D(dimensions.x, dimensions.y, textureData.TextureMipLevels > 1, 1, format, flag, bgfx::copy(buffer, info.storageSize)).idx;

        SetBgfxResource(entity, {textureHandle});
    } else {
        bgfx::updateTexture2D(handle, 0, 0, 0, 0, dimensions.x, dimensions.y, bgfx::copy(buffer, info.storageSize));
    }

    free(buffer);
    data.size = info.storageSize;

    SetBgfxTexture2D(entity, data);
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    for(auto& entity : invalidatedTextures) {
        ValidateTexture(entity);
    }

	invalidatedTextures.clear();
}

static void OnStreamChanged(Entity entity, const Stream& oldData, const Stream& newData) {
    auto owner = GetOwnership(entity).Owner;
    if(HasComponent(owner, ComponentOf_BgfxTexture2D())) {
        invalidatedTextures.insert(owner);
    }
}

static void OnTexture2DChanged(Entity entity, const Texture2D& oldData, const Texture2D& newData) {
    auto owner = GetOwnership(entity).Owner;
    if(HasComponent(owner, ComponentOf_BgfxTexture2D())) {
        invalidatedTextures.insert(owner);
    }
}

static void OnTextureChanged(Entity entity, const Texture& oldData, const Texture& newData) {
    auto owner = GetOwnership(entity).Owner;
    if(HasComponent(owner, ComponentOf_BgfxTexture2D())) {
        invalidatedTextures.insert(owner);
    }
}

BeginUnit(BgfxTexture2D)
    BeginComponent(BgfxTexture2D)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSystem(OnStreamChanged, ComponentOf_Stream())
    RegisterSystem(OnTexture2DChanged, ComponentOf_Texture2D())
    RegisterSystem(OnTextureChanged, ComponentOf_Texture())
    RegisterSystem(OnBgfxResourceChanged, ComponentOf_BgfxResource())
    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_ResourceSubmission)
EndUnit()
