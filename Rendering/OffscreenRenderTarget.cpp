//
// Created by Kim Johannsen on 19-03-2018.
//

#include "OffscreenRenderTarget.h"
#include "RenderTarget.h"
#include "Texture2D.h"
#include "Texture.h"

struct OffscreenRenderTarget {
    Vector(OffscreenRenderTargetTextures, Entity, 8)
};

LocalFunction(OnOffscreenRenderTargetTexturesChanged, void, Entity stage, Entity oldTexture, Entity newTexture) {
    auto renderTarget = GetOwner(stage);

    SetTextureSize2D(newTexture, GetRenderTargetSize(renderTarget));
}

LocalFunction(OnRenderTargetSizeChanged, void, Entity entity, v2i oldSize, v2i newSize) {
    auto data = GetOffscreenRenderTargetData(entity);
    if(data) {
        auto numStages = data->OffscreenRenderTargetTextures.Count;
        auto stages = GetVector(data->OffscreenRenderTargetTextures);
        for(auto i = 0; i < numStages; ++i) {
            SetTextureSize2D(stages[i], newSize);
        }
    }
}

BeginUnit(OffscreenRenderTarget)
    BeginComponent(OffscreenRenderTarget)
        RegisterArrayProperty(Texture2D, OffscreenRenderTargetTextures)
    EndComponent()

    RegisterSubscription(RenderTargetSizeChanged, OnRenderTargetSizeChanged, 0)
    RegisterSubscription(OffscreenRenderTargetTexturesChanged, OnOffscreenRenderTargetTexturesChanged, 0)
EndUnit()
