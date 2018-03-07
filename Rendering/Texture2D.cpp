//
// Created by Kim Johannsen on 14/01/2018.
//

#include "Texture2D.h"

struct Texture2D {
    v2i TextureSize2D;
};

DefineComponent(Texture2D)
    DefineProperty(v2i, TextureSize2D)
EndComponent()

DefineComponentProperty(Texture2D, v2i, TextureSize2D)


