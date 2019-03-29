//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxUniform.h"
#include "BgfxResource.h"
#include <bgfx/bgfx.h>
#include <Rendering/Uniform.h>
#include <Rendering/RenderContext.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include <Core/Identification.h>
#include <Core/Algorithms.h>

struct BgfxUniform {
};

LocalFunction(OnUniformRemoved, void, Entity component, Entity entity) {
	auto resourceData = GetBgfxResourceData(entity);
    bgfx::UniformHandle handle = { resourceData->BgfxResourceHandle };

    if(bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }

	resourceData->BgfxResourceHandle = bgfx::kInvalidHandle;
}

static eastl::set<Entity> invalidatedUniforms;

LocalFunction(OnUniformValidation, void) {
    for(auto& entity : invalidatedUniforms) {
        // Eventually free old buffers
        OnUniformRemoved(0, entity);

        // Create or update unifom
        bgfx::UniformType::Enum type;

        auto property = GetUniformEntityProperty(entity);

        auto propertyType = GetPropertyType(property);
		
        switch (propertyType) {
            case TypeOf_s8:
            case TypeOf_u8:
            case TypeOf_s16:
            case TypeOf_u16:
            case TypeOf_s32:
            case TypeOf_u32:
            case TypeOf_double:
            case TypeOf_float:
            case TypeOf_v2f:
            case TypeOf_v3f:
            case TypeOf_v4f:
            case TypeOf_v2i:
            case TypeOf_v3i:
            case TypeOf_v4i:
            case TypeOf_rgb8:
            case TypeOf_rgba8:
            case TypeOf_rgb32:
            case TypeOf_rgba32:
                type = bgfx::UniformType::Vec4;
                break;
            case TypeOf_Entity:
                type = bgfx::UniformType::Int1;
                break;
            default:
                Log(entity, LogSeverity_Error, "Unsupported uniform property type: %s (uniform %s)", GetTypeName(propertyType), GetIdentification(entity).Uuid);
                break;
        }

        auto name = GetUniformIdentifier(entity);
        auto arrayCount = Max((u32)1, GetUniformArrayCount(entity));
        SetBgfxResourceHandle(entity, bgfx::createUniform(name, type, arrayCount).idx);
    }

    invalidatedUniforms.clear();
}

LocalFunction(Invalidate, void, Entity entity) {
    invalidatedUniforms.insert(entity);
}

LocalFunction(OnUniformAdded, void, Entity component, Entity entity) {
	invalidatedUniforms.insert(entity);
}

BeginUnit(BgfxUniform)
    BeginComponent(BgfxUniform)
        RegisterBase(BgfxResource)
    EndComponent()

	RegisterSubscription(EventOf_EntityComponentAdded(), OnUniformAdded, ComponentOf_BgfxUniform())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnUniformRemoved, ComponentOf_BgfxUniform())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UniformEntityProperty()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UniformIdentifier()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UniformArrayCount()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UniformElementProperty()), Invalidate, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnUniformValidation, AppLoopOf_ResourceSubmission())
EndUnit()
