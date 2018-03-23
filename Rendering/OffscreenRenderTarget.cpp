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

DefineComponent(OffscreenRenderTarget)
    DefineProperty(Entity, RenderTargetTexture0)
    DefineProperty(Entity, RenderTargetTexture1)
    DefineProperty(Entity, RenderTargetTexture2)
    DefineProperty(Entity, RenderTargetTexture3)
    DefineProperty(Entity, RenderTargetTexture4)
    DefineProperty(Entity, RenderTargetTexture5)
    DefineProperty(Entity, RenderTargetTexture6)
    DefineProperty(Entity, RenderTargetTexture7)
EndComponent()

DefineComponentPropertyReactive(OffscreenRenderTarget, Entity, RenderTargetTexture0)
DefineComponentPropertyReactive(OffscreenRenderTarget, Entity, RenderTargetTexture1)
DefineComponentPropertyReactive(OffscreenRenderTarget, Entity, RenderTargetTexture2)
DefineComponentPropertyReactive(OffscreenRenderTarget, Entity, RenderTargetTexture3)
DefineComponentPropertyReactive(OffscreenRenderTarget, Entity, RenderTargetTexture4)
DefineComponentPropertyReactive(OffscreenRenderTarget, Entity, RenderTargetTexture5)
DefineComponentPropertyReactive(OffscreenRenderTarget, Entity, RenderTargetTexture6)
DefineComponentPropertyReactive(OffscreenRenderTarget, Entity, RenderTargetTexture7)

DefineService(OffscreenRenderTarget)
EndService()

static void OnChanged(Entity entity) {
    auto data = GetOffscreenRenderTarget(entity);

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

static void OnRenderTargetSizeChanged(Entity entity, v2i oldSize, v2i newSize) {
    if(HasOffscreenRenderTarget(entity)) OnChanged(entity);
}

static bool ServiceStart() {
    SubscribeRenderTargetSizeChanged(OnRenderTargetSizeChanged);
    SubscribeOffscreenRenderTargetChanged(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeRenderTargetSizeChanged(OnRenderTargetSizeChanged);
    UnsubscribeOffscreenRenderTargetChanged(OnChanged);
    return true;
}
