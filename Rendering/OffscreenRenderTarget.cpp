//
// Created by Kim Johannsen on 19-03-2018.
//

#include "OffscreenRenderTarget.h"
#include "RenderTarget.h"
#include "Texture2D.h"

struct OffscreenRenderTarget {
    Entity RenderTargetTexture0,
            RenderTargetTexture1,
            RenderTargetTexture2,
            RenderTargetTexture3,
            RenderTargetTexture4,
            RenderTargetTexture5,
            RenderTargetTexture6,
            RenderTargetTexture7;
};

BeginUnit(OffscreenRenderTarget)
    BeginComponent(OffscreenRenderTarget)
    RegisterProperty(Entity, RenderTargetTexture0)
    RegisterProperty(Entity, RenderTargetTexture1)
    RegisterProperty(Entity, RenderTargetTexture2)
    RegisterProperty(Entity, RenderTargetTexture3)
    RegisterProperty(Entity, RenderTargetTexture4)
    RegisterProperty(Entity, RenderTargetTexture5)
    RegisterProperty(Entity, RenderTargetTexture6)
    RegisterProperty(Entity, RenderTargetTexture7)
EndComponent()

RegisterProperty(Entity, RenderTargetTexture0)
RegisterProperty(Entity, RenderTargetTexture1)
RegisterProperty(Entity, RenderTargetTexture2)
RegisterProperty(Entity, RenderTargetTexture3)
RegisterProperty(Entity, RenderTargetTexture4)
RegisterProperty(Entity, RenderTargetTexture5)
RegisterProperty(Entity, RenderTargetTexture6)
RegisterProperty(Entity, RenderTargetTexture7)

LocalFunction(OnChanged, void, Entity entity) {
    auto data = GetOffscreenRenderTargetData(entity);

    Entity textures[] = {
            data->RenderTargetTexture0,
            data->RenderTargetTexture1,
            data->RenderTargetTexture2,
            data->RenderTargetTexture3,
            data->RenderTargetTexture4,
            data->RenderTargetTexture5,
            data->RenderTargetTexture6,
            data->RenderTargetTexture7
    };

    for(auto i = 0; i < 8; ++i) {
        if(IsEntityValid(textures[i])) {
            auto rtSize = GetRenderTargetSize(entity);
            auto texSize = GetTextureSize2D(textures[i]);
            if(memcmp(&rtSize, &texSize, sizeof(v2i)) != 0) {
                SetTextureSize2D(textures[i], rtSize);
            }
        }
    }
}

LocalFunction(OnRenderTargetSizeChanged, void, Entity entity, v2i oldSize, v2i newSize) {
    if(HasComponent(entity, ComponentOf_OffscreenRenderTarget())) OnChanged(entity);
}

DefineService(OffscreenRenderTarget)
        RegisterSubscription(RenderTargetSizeChanged, OnRenderTargetSizeChanged, 0)
        RegisterSubscription(OffscreenRenderTargetChanged, OnChanged, 0)
EndService()
