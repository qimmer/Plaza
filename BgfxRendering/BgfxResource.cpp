//
// Created by Kim on 22-08-2018.
//

#include "BgfxResource.h"
#include <Foundation/Stream.h>
#include <bgfx/bgfx.h>
#include <Core/Debug.h>

LocalFunction(OnBgfxResourceAdded, void, Entity component, Entity entity) {
    auto data = GetBgfxResourceData(entity);

    data->BgfxResourceHandle = bgfx::kInvalidHandle;
}

LocalFunction(OnBgfxResourceRemoved, void, Entity component, Entity entity) {
	auto data = GetBgfxResourceData(entity);

	Assert(entity, data->BgfxResourceHandle == bgfx::kInvalidHandle);
}

BeginUnit(BgfxResource)
    BeginComponent(BgfxResource)
        RegisterProperty(u16, BgfxResourceHandle)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnBgfxResourceAdded, ComponentOf_BgfxResource())
	RegisterSubscription(EventOf_EntityComponentRemoved(), OnBgfxResourceRemoved, ComponentOf_BgfxResource())
EndUnit()