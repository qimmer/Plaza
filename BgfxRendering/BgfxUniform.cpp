//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxUniform.h"
#include "BgfxResource.h"
#include <bgfx/bgfx.h>
#include <Rendering/Uniform.h>
#include <Core/Debug.h>
#include <Core/Identification.h>
#include <Foundation/Invalidation.h>
#include <Core/Algorithms.h>

struct BgfxUniform {
};

LocalFunction(OnUniformRemoved, void, Entity entity) {
    bgfx::UniformHandle handle = { GetBgfxResourceHandle(entity) };

    if(bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }
}

LocalFunction(OnUniformValidation, void, Entity component) {
    for_entity(entity, data, BgfxUniform) {
        if(!IsDirty(entity)) continue;

        // Eventually free old buffers
        OnUniformRemoved(entity);

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
            case TypeOf_m3x3f:
            case TypeOf_m4x4f:
                type = bgfx::UniformType::Mat4;
                break;
            case TypeOf_Entity:
                type = bgfx::UniformType::Int1;
                break;
            default:
                Log(entity, LogSeverity_Error, "Unsupported uniform property type: %s (uniform %s)", GetTypeName(propertyType), GetUuid(entity));
                break;
        }

        auto name = GetUniformIdentifier(entity);
        auto arrayCount = Max((u32)1, GetUniformArrayCount(entity));
        SetBgfxResourceHandle(entity, bgfx::createUniform(name, type, arrayCount).idx);
    }
}

BeginUnit(BgfxUniform)
    BeginComponent(BgfxUniform)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentRemoved(), OnUniformRemoved, ComponentOf_BgfxUniform())
    RegisterSubscription(EventOf_Validate(), OnUniformValidation, ComponentOf_Uniform())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UniformEntityProperty()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UniformIdentifier()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UniformArrayCount()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UniformElementProperty()), Invalidate, 0)
EndUnit()
