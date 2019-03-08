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
#include <Foundation/AppLoop.h>

struct BgfxTexture2D {
    u32 size, flag;
};

static eastl::set<Entity> invalidatedTextures;

LocalFunction(OnTexture2DRemoved, void, Entity component, Entity entity) {
	auto resourceData = GetBgfxResourceData(entity);
    bgfx::TextureHandle handle = { resourceData->BgfxResourceHandle };

    if(bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }

	resourceData->BgfxResourceHandle = bgfx::kInvalidHandle;
}

LocalFunction(OnTexture2DAdded, void, Entity component, Entity entity) {
	invalidatedTextures.insert(entity);
}

static void ValidateTexture(Entity entity) {
    auto data = GetBgfxTexture2DData(entity);

    bgfx::TextureHandle handle = { GetBgfxResourceHandle(entity) };

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
        OnTexture2DRemoved(0, entity);

        auto textureHandle = (flag & TextureFlag_READ_BACK || flag & TextureFlag_RT)
            ? bgfx::createTexture2D(dimensions.x, dimensions.y, GetTextureMipLevels(entity) > 1, 1, format, flag).idx
            : bgfx::createTexture2D(dimensions.x, dimensions.y, GetTextureMipLevels(entity) > 1, 1, format, flag, bgfx::copy(buffer, info.storageSize)).idx;



        SetBgfxResourceHandle(entity, textureHandle);
    } else {
        bgfx::updateTexture2D(handle, 0, 0, 0, 0, dimensions.x, dimensions.y, bgfx::copy(buffer, info.storageSize));
    }

    free(buffer);
    data->size = info.storageSize;
}

LocalFunction(OnValidation, void, Entity component) {
    for(auto& entity : invalidatedTextures) {
        ValidateTexture(entity);
    }

	invalidatedTextures.clear();
}

LocalFunction(Invalidate, void, Entity entity) {
    invalidatedTextures.insert(entity);
}

LocalFunction(OnStreamContentChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxTexture2D())) {
        invalidatedTextures.insert(entity);
    }
}

BeginUnit(BgfxTexture2D)
    BeginComponent(BgfxTexture2D)
        RegisterBase(BgfxResource)
    EndComponent()

	RegisterSubscription(EventOf_EntityComponentAdded(), OnTexture2DAdded, ComponentOf_BgfxTexture2D())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnTexture2DRemoved, ComponentOf_BgfxTexture2D())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureSize2D()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureFormat()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureFlag()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureDynamic()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureMipLevels()), Invalidate, 0)
    RegisterSubscription(EventOf_StreamContentChanged(), OnStreamContentChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnValidation, AppLoopOf_ResourceSubmission())
EndUnit()
