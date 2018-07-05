//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Texture2D.h"
#include "Texture.h"

struct Texture2D {
    v2i TextureSize2D;
};

DefineComponent(Texture2D)
    DefinePropertyReactive(v2i, TextureSize2D)
EndComponent()

DefineComponentPropertyReactive(Texture2D, v2i, TextureSize2D)

static void OnTexture2DChanged(Entity texture) {
    FireNativeEvent(TextureChanged, texture);
}

DefineService(Texture2D)
    Subscribe(Texture2DChanged, OnTexture2DChanged)
EndService()