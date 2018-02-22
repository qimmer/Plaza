//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Foundation/Invalidation.h>
#include "Texture.h"


    struct Texture {
        Texture() {
            memset(this, 0, sizeof(Texture));
        }
        u16 TextureFormat;
        bool TextureDynamic;
        Entity TextureDataStream;
        u8 TextureMipLevels;
    };

    DefineComponent(Texture)
        Dependency(Invalidation)
    EndComponent()

    DefineComponentProperty(Texture, Entity, TextureDataStream)
    DefineComponentProperty(Texture, u16, TextureFormat)
    DefineComponentProperty(Texture, bool, TextureDynamic)
    DefineComponentProperty(Texture, u8, TextureMipLevels)
