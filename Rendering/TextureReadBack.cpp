//
// Created by Kim on 30-10-2018.
//

#include "TextureReadBack.h"
#include "Texture2D.h"
#include <Foundation/MemoryStream.h>

BeginUnit(TextureReadBack)
    BeginComponent(TextureReadBack)
        RegisterReferenceProperty(Texture2D, TextureReadBackSourceTexture)
        BeginChildProperty(TextureReadBackBuffer)
        EndChildProperty()
        BeginChildProperty(TextureReadBackBlitTexture)
        EndChildProperty()
        RegisterProperty(u32, TextureReadBackPendingReadBacks)
    EndComponent()
EndUnit()