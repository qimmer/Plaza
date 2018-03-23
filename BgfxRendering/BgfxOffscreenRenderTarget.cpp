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

DefineComponent(BgfxOffscreenRenderTarget)
EndComponent()

DefineService(BgfxOffscreenRenderTarget)
EndService()

static void OnChanged(Entity entity) {
    if(HasBgfxOffscreenRenderTarget(entity)) {
        GetBgfxOffscreenRenderTarget(entity)->invalidated = true;
    }
}

void OnOffscreenRenderTargetRemoved(Entity entity) {
    auto data = GetBgfxOffscreenRenderTarget(entity);

    if(bgfx::isValid(data->handle)) {
        bgfx::destroy(data->handle);
        data->handle = BGFX_INVALID_HANDLE;
    }
}

static bool ServiceStart() {
    SubscribeBgfxOffscreenRenderTargetRemoved(OnOffscreenRenderTargetRemoved);
    SubscribeOffscreenRenderTargetChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeBgfxOffscreenRenderTargetRemoved(OnOffscreenRenderTargetRemoved);
    UnsubscribeOffscreenRenderTargetChanged(OnChanged);
    return true;
}

u16 GetBgfxOffscreenRenderTargetHandle(Entity entity) {
    auto data = GetBgfxOffscreenRenderTarget(entity);

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

