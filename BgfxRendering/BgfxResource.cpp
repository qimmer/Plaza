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

    if(data) {
        Assert(entity, data->BgfxResourceHandle == bgfx::kInvalidHandle);
    }
}

LocalFunction(InvalidateResource, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxResource())) {
        Invalidate(entity);
    }
}

BeginUnit(BgfxResource)
    BeginComponent(BgfxResource)
        RegisterBase(Invalidation)

        RegisterProperty(u16, BgfxResourceHandle)
    EndComponent()

    RegisterSubscription(StreamContentChanged, InvalidateResource, 0)
    RegisterSubscription(StreamPathChanged, InvalidateResource, 0)
    RegisterSubscription(EntityComponentAdded, OnBgfxResourceAdded, ComponentOf_BgfxResource())
    RegisterSubscription(EntityComponentRemoved, OnBgfxResourceRemoved, ComponentOf_BgfxResource())
EndUnit()