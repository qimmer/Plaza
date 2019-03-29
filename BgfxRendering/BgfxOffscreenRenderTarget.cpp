//
// Created by Kim Johannsen on 19-03-2018.
//

#include <bgfx/bgfx.h>
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include <Rendering/RenderContext.h>
#include <Foundation/AppLoop.h>

#include "BgfxOffscreenRenderTarget.h"
#include "BgfxTexture2D.h"
#include "BgfxResource.h"

static eastl::set<Entity> invalidatedTargets;

struct BgfxOffscreenRenderTarget {
};

LocalFunction(OnBgfxOffscreenRenderTargetRemoved, void, Entity component, Entity entity) {
	auto resourceData = GetBgfxResourceData(entity);
    bgfx::FrameBufferHandle handle = { resourceData->BgfxResourceHandle };
	
    if(bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }

	resourceData->BgfxResourceHandle = bgfx::kInvalidHandle;
}

LocalFunction(OnValidation, void, Entity component) {
    for(auto& entity : invalidatedTargets) {
        OnBgfxOffscreenRenderTargetRemoved(0, entity);

        auto stages = GetOffscreenRenderTargetTextures(entity);
        auto targets = (bgfx::TextureHandle*)alloca(stages.size() * sizeof(bgfx::TextureHandle));

        for(auto i = 0; i < stages.size(); ++i) {
            targets[i] = {
                GetBgfxResourceHandle(stages[i])
            };
        }

        SetBgfxResourceHandle(entity, bgfx::createFrameBuffer(stages.size(), targets).idx);
    }

    invalidatedTargets.clear();
}

LocalFunction(InvalidateTarget, void, Entity entity) {
    invalidatedTargets.insert(entity);
}

LocalFunction(InvalidateTexture, void, Entity entity) {
    auto owner = GetOwnership(entity).Owner;
    if(HasComponent(owner, ComponentOf_BgfxOffscreenRenderTarget())) {
        InvalidateTarget(owner);
    }
}

BeginUnit(BgfxOffscreenRenderTarget)
    BeginComponent(BgfxOffscreenRenderTarget)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentRemoved(), OnBgfxOffscreenRenderTargetRemoved, ComponentOf_BgfxOffscreenRenderTarget())

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnValidation, AppLoopOf_ResourceSubmission())

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_OffscreenRenderTargetTextures()), InvalidateTarget, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureSize2D()), InvalidateTexture, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureFormat()), InvalidateTexture, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureFlag()), InvalidateTexture, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextureDynamic()), InvalidateTexture, 0)
EndUnit()
