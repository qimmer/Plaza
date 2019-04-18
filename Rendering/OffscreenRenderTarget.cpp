//
// Created by Kim Johannsen on 19-03-2018.
//

#include "OffscreenRenderTarget.h"
#include "RenderTarget.h"
#include "Texture2D.h"
#include "Texture.h"

static void OnOwnerChanged(Entity texture, const Ownership& oldData, const Ownership& newData) {
    if(newData.OwnerProperty == PropertyOf_OffscreenRenderTargetTextures()) {
        SetTexture2D(texture, {GetRenderTarget(newData.Owner).RenderTargetSize});
    }
}

static void OnRenderTargetChanged(Entity entity, const RenderTarget& oldData, const RenderTarget& newData) {
    auto offscreenRenderTargetData = GetOffscreenRenderTarget(entity);

    if(offscreenRenderTargetData.OffscreenRenderTargetTextures.GetSize()) {
        for (auto stage : offscreenRenderTargetData.OffscreenRenderTargetTextures) {
            SetTexture2D(stage, {newData.RenderTargetSize});
        }
    }
}

BeginUnit(OffscreenRenderTarget)
    BeginComponent(OffscreenRenderTarget)
        RegisterArrayProperty(Texture2D, OffscreenRenderTargetTextures)
    EndComponent()

    RegisterSystem(OnOwnerChanged, ComponentOf_Ownership())
    RegisterSystem(OnRenderTargetChanged, ComponentOf_RenderTarget())
EndUnit()
