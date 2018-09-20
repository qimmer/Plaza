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

LocalFunction(OnValidation, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxUniform())) {
        // Eventually free old buffers
        OnUniformRemoved(entity);

        // Create or update unifom
        bgfx::UniformType::Enum type;

        if(GetUniformType(entity) == TypeOf_v4f) {
            type = bgfx::UniformType::Vec4;
        } else if(GetUniformType(entity) == TypeOf_m4x4f) {
            type = bgfx::UniformType::Mat4;
        } else if(GetUniformType(entity) == TypeOf_Entity) {
            type = bgfx::UniformType::Int1;
        } else {
            Log(entity, LogSeverity_Error, "Unsupported uniform type: %s", GetTypeName(GetUniformType(entity)));

        }

        auto name = GetName(entity);
        auto arrayCount = Max((u32)1, GetUniformArrayCount(entity));
        SetBgfxResourceHandle(entity, bgfx::createUniform(name, type, arrayCount).idx);
    }
}

BeginUnit(BgfxUniform)
    BeginComponent(BgfxUniform)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSubscription(EntityComponentRemoved, OnUniformRemoved, ComponentOf_BgfxUniform())
    RegisterSubscription(Validate, OnValidation, 0)
    RegisterSubscription(UniformTypeChanged, Invalidate, 0)
    RegisterSubscription(UniformIdentifierChanged, Invalidate, 0)
EndUnit()
