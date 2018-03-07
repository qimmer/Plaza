//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Invalidation.h>
#include <Foundation/Stream.h>
#include "Texture.h"

struct Texture {
    Texture() : TextureMipLevels(1), TextureFlag(TextureFlag_NONE) {}

    u32 TextureFlag;
    u16 TextureFormat;
    bool TextureDynamic;
    u8 TextureMipLevels;
};

DefineComponent(Texture)
    Dependency(Invalidation)
    Dependency(Stream)
    DefineProperty(u16, TextureFormat)
    DefineProperty(u32, TextureFlag)
    DefineProperty(bool, TextureDynamic)
    DefineProperty(u8, TextureMipLevels)
EndComponent()

DefineComponentProperty(Texture, u32, TextureFlag)
DefineComponentProperty(Texture, u16, TextureFormat)
DefineComponentProperty(Texture, bool, TextureDynamic)
DefineComponentProperty(Texture, u8, TextureMipLevels)
