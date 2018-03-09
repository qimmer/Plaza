//
// Created by Kim Johannsen on 14/01/2018.
//

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
    Dependency(Stream)
    DefineProperty(u16, TextureFormat)
    DefineProperty(u32, TextureFlag)
    DefineProperty(bool, TextureDynamic)
    DefineProperty(u8, TextureMipLevels)
EndComponent()

DefineComponentPropertyReactive(Texture, u32, TextureFlag)
DefineComponentPropertyReactive(Texture, u16, TextureFormat)
DefineComponentPropertyReactive(Texture, bool, TextureDynamic)
DefineComponentPropertyReactive(Texture, u8, TextureMipLevels)
