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

LocalFunction(OnValidation, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_Texture())) {
        auto owner = GetOwner(entity);
        if(HasComponent(owner, ComponentOf_BgfxOffscreenRenderTarget())) {
            Invalidate(owner);
        }
    }

    if(HasComponent(entity, ComponentOf_BgfxOffscreenRenderTarget())) {
        bgfx::TextureHandle handle = { GetBgfxResourceHandle(entity) };

        OnBgfxOffscreenRenderTargetRemoved(entity);

        auto numStages = GetNumOffscreenRenderTargetTextures(entity);
        auto stages = GetOffscreenRenderTargetTextures(entity);
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

    RegisterSubscription(EntityComponentRemoved, OnBgfxOffscreenRenderTargetRemoved, ComponentOf_BgfxOffscreenRenderTarget())
    RegisterSubscription(Validate, OnValidation, 0)
    RegisterSubscription(OffscreenRenderTargetTexturesChanged, Invalidate, 0)
EndUnit()
