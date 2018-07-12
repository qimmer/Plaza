//
// Created by Kim Johannsen on 19-03-2018.
//

#include <bgfx/bgfx.h>
#include <Rendering/OffscreenRenderTarget.h>
#include "BgfxOffscreenRenderTarget.h"
#include "BgfxTexture2D.h"

struct BgfxOffscreenRenderTarget {
    BgfxOffscreenRenderTarget() :
            handle(BGFX_INVALID_HANDLE),
            invalidated(true) {}

    bgfx::FrameBufferHandle handle;
    bool invalidated;
};

BeginUnit(BgfxOffscreenRenderTarget)
    BeginComponent(BgfxOffscreenRenderTarget)
EndComponent()

LocalFunction(OnChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxOffscreenRenderTarget())) {
        GetBgfxOffscreenRenderTargetData(entity)->invalidated = true;
    }
}

void OnOffscreenRenderTargetRemoved(Entity entity) {
    auto data = GetBgfxOffscreenRenderTargetData(entity);

    if(bgfx::isValid(data->handle)) {
        bgfx::destroy(data->handle);
        data->handle = BGFX_INVALID_HANDLE;
    }
}

u16 GetBgfxOffscreenRenderTargetHandle(Entity entity) {
    auto data = GetBgfxOffscreenRenderTargetData(entity);

    if(data->invalidated) {
        if(bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }

        bgfx::TextureHandle targets[8];
        targets[0] = {GetBgfxTexture2DHandle(GetRenderTargetTexture0(entity))};
        targets[1] = {GetBgfxTexture2DHandle(GetRenderTargetTexture1(entity))};
        targets[2] = {GetBgfxTexture2DHandle(GetRenderTargetTexture2(entity))};
        targets[3] = {GetBgfxTexture2DHandle(GetRenderTargetTexture3(entity))};
        targets[4] = {GetBgfxTexture2DHandle(GetRenderTargetTexture4(entity))};
        targets[5] = {GetBgfxTexture2DHandle(GetRenderTargetTexture5(entity))};
        targets[6] = {GetBgfxTexture2DHandle(GetRenderTargetTexture6(entity))};
        targets[7] = {GetBgfxTexture2DHandle(GetRenderTargetTexture7(entity))};

        u8 numTargets = 0;
        while(numTargets < 8 && targets[numTargets].idx != bgfx::kInvalidHandle) ++numTargets;

        data->handle = bgfx::createFrameBuffer(numTargets, targets);
        data->invalidated = false;
    }

    return data->handle.idx;
}

DefineService(BgfxOffscreenRenderTarget)
        RegisterSubscription(BgfxOffscreenRenderTargetRemoved, OnOffscreenRenderTargetRemoved, 0)
        RegisterSubscription(OffscreenRenderTargetChanged, OnChanged, 0)
EndService()
