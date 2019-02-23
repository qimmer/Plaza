//
// Created by Kim Johannsen on 19-03-2018.
//

#include <bgfx/bgfx.h>
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture.h>
#include <Foundation/Invalidation.h>
#include "BgfxOffscreenRenderTarget.h"
#include "BgfxTexture2D.h"
#include "BgfxResource.h"

struct BgfxOffscreenRenderTarget {
};

LocalFunction(OnBgfxOffscreenRenderTargetRemoved, void, Entity entity) {
    bgfx::FrameBufferHandle handle = { GetBgfxResourceHandle(entity) };

    if(bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }
}

LocalFunction(OnTextureValidation, void, Entity component) {
    for_entity(entity, data, BgfxOffscreenRenderTarget) {
        if(!IsDirty(entity)) continue;

        auto owner = GetOwner(entity);
        if(HasComponent(owner, ComponentOf_BgfxOffscreenRenderTarget())) {
            Invalidate(owner);
        }
    }
}

LocalFunction(OnOffscreenRenderTargetValidation, void, Entity component) {
    for_entity(entity, data, BgfxOffscreenRenderTarget) {
        if(!IsDirty(entity)) continue;

        bgfx::TextureHandle handle = { GetBgfxResourceHandle(entity) };

        OnBgfxOffscreenRenderTargetRemoved(entity);

        u32 numStages = 0;
        auto stages = GetOffscreenRenderTargetTextures(entity, &numStages);
        auto targets = (bgfx::TextureHandle*)alloca(numStages * sizeof(bgfx::TextureHandle));

        for(auto i = 0; i < numStages; ++i) {
            targets[i] = {
                GetBgfxResourceHandle(stages[i])
            };
        }

        SetBgfxResourceHandle(entity, bgfx::createFrameBuffer(numStages, targets).idx);
    }
}

BeginUnit(BgfxOffscreenRenderTarget)
    BeginComponent(BgfxOffscreenRenderTarget)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentRemoved(), OnBgfxOffscreenRenderTargetRemoved, ComponentOf_BgfxOffscreenRenderTarget())
    RegisterSubscription(EventOf_Validate(), OnTextureValidation, ComponentOf_Texture())
    RegisterSubscription(EventOf_Validate(), OnOffscreenRenderTargetValidation, ComponentOf_OffscreenRenderTarget())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_OffscreenRenderTargetTextures()), Invalidate, 0)
EndUnit()
