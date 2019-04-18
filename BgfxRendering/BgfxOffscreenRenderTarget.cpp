//
// Created by Kim Johannsen on 19-03-2018.
//

#include <bgfx/bgfx.h>
#include <Rendering/OffscreenRenderTarget.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include <Rendering/RenderContext.h>
#include <Foundation/AppLoop.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/RenderingModule.h>

#include "BgfxOffscreenRenderTarget.h"
#include "BgfxTexture2D.h"
#include "BgfxResource.h"

static eastl::set<Entity> invalidatedTargets;

struct BgfxOffscreenRenderTarget {
};

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

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    for(auto& entity : invalidatedTargets) {
        auto stages = GetOffscreenRenderTarget(entity).OffscreenRenderTargetTextures;
        auto targets = (bgfx::TextureHandle*)alloca(stages.GetSize() * sizeof(bgfx::TextureHandle));

        for(auto i = 0; i < stages.GetSize(); ++i) {
            targets[i] = {
                GetBgfxResource(stages[i]).BgfxResourceHandle
            };
        }

        SetBgfxResource(entity, {bgfx::createFrameBuffer(stages.GetSize(), targets).idx});
    }

    invalidatedTargets.clear();
}

static void OnOffscreenRenderTargetChanged(Entity entity, const OffscreenRenderTarget& oldData, const OffscreenRenderTarget& newData) {
    invalidatedTargets.insert(entity);
}

static void OnRenderTargetChanged(Entity entity, const RenderTarget& oldData, const RenderTarget& newData) {
    if(HasComponent(entity, ComponentOf_BgfxOffscreenRenderTarget())) {
        invalidatedTargets.insert(entity);
    }
}

static void OnTextureChanged(Entity entity, const Texture& oldData, const Texture& newData) {
    auto owner = GetOwnership(entity).Owner;
    if(HasComponent(owner, ComponentOf_BgfxOffscreenRenderTarget())) {
        invalidatedTargets.insert(owner);
    }
}

static void OnTexture2DChanged(Entity entity, const Texture2D& oldData, const Texture2D& newData) {
    auto owner = GetOwnership(entity).Owner;
    if(HasComponent(owner, ComponentOf_BgfxOffscreenRenderTarget())) {
        invalidatedTargets.insert(owner);
    }
}

BeginUnit(BgfxOffscreenRenderTarget)
    BeginComponent(BgfxOffscreenRenderTarget)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSystem(OnTextureChanged, ComponentOf_Texture())
    RegisterSystem(OnTexture2DChanged, ComponentOf_Texture2D())
    RegisterSystem(OnRenderTargetChanged, ComponentOf_RenderTarget())
    RegisterSystem(OnOffscreenRenderTargetChanged, ComponentOf_OffscreenRenderTarget())
    RegisterSystem(OnBgfxResourceChanged, ComponentOf_BgfxResource())
    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_ResourceSubmission)
EndUnit()
