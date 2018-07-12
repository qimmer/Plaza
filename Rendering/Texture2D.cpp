//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Texture2D.h"
#include "Texture.h"

struct Texture2D {
    v2i TextureSize2D;
};

BeginUnit(Texture2D)
    BeginComponent(Texture2D)
    RegisterProperty(v2i, TextureSize2D)
EndComponent()

RegisterProperty(v2i, TextureSize2D)

LocalFunction(OnTexture2DChanged, void, Entity texture) {
    FireEvent(EventOf_TextureChanged(), texture);
}

DefineService(Texture2D)
    RegisterSubscription(Texture2DChanged, OnTexture2DChanged, 0)
EndService()