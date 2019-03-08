//
// Created by Kim Johannsen on 19-03-2018.
//

#include "OffscreenRenderTarget.h"
#include "RenderTarget.h"
#include "Texture2D.h"
#include "Texture.h"

struct OffscreenRenderTarget {
};

LocalFunction(OnOffscreenRenderTargetTexturesChanged, void, Entity stage, Entity oldTexture, Entity newTexture) {
    auto renderTarget = GetOwner(stage);

    SetTextureSize2D(newTexture, GetRenderTargetSize(renderTarget));
}

LocalFunction(OnRenderTargetSizeChanged, void, Entity entity, v2i oldSize, v2i newSize) {
    auto& stages = GetOffscreenRenderTargetTextures(entity);
	for (auto i = 0; i < stages.size(); ++i) {
		SetTextureSize2D(stages[i], newSize);
	}
}

BeginUnit(OffscreenRenderTarget)
    BeginComponent(OffscreenRenderTarget)
        RegisterArrayProperty(Texture2D, OffscreenRenderTargetTextures)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderTargetSize()), OnRenderTargetSizeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_OffscreenRenderTargetTextures()), OnOffscreenRenderTargetTexturesChanged, 0)
EndUnit()
