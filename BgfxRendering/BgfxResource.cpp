//
// Created by Kim on 22-08-2018.
//

#include "BgfxResource.h"
#include <Foundation/Stream.h>
#include <bgfx/bgfx.h>
#include <Core/Debug.h>

BeginUnit(BgfxResource)
    BeginComponent(BgfxResource)
        defaultValue.BgfxResourceHandle = bgfx::kInvalidHandle;

        RegisterProperty(u16, BgfxResourceHandle)
    EndComponent()
EndUnit()