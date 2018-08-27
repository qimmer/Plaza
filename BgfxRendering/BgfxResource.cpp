//
// Created by Kim on 22-08-2018.
//

#include "BgfxResource.h"
#include <Foundation/Stream.h>
#include <bgfx/bgfx.h>
#include <Core/Debug.h>
#include <Foundation/Invalidation.h>

struct BgfxResource {
    u16 BgfxResourceHandle;
};

LocalFunction(OnBgfxResourceAdded, void, Entity component, Entity entity) {
    auto data = GetBgfxResourceData(entity);

    data->BgfxResourceHandle = bgfx::kInvalidHandle;
}

LocalFunction(OnBgfxResourceRemoved, void, Entity entity) {
    auto data = GetBgfxResourceData(entity);

    Assert(entity, data->BgfxResourceHandle == bgfx::kInvalidHandle);
}

BeginUnit(BgfxResource)
    BeginComponent(BgfxResource)
        RegisterBase(Invalidation)

        RegisterProperty(u16, BgfxResourceHandle)
    EndComponent()

    RegisterSubscription(StreamContentChanged, Invalidate, 0)
    RegisterSubscription(StreamPathChanged, Invalidate, 0)
EndUnit()