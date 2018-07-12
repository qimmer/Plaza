//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxUniform.h"
#include <bgfx/bgfx.h>
#include <Rendering/Uniform.h>


struct BgfxUniform {
    BgfxUniform() :
            handle(BGFX_INVALID_HANDLE), invalidated(true) {}

    bgfx::UniformHandle handle;
    bool invalidated;
};

BeginUnit(BgfxUniform)
    BeginComponent(BgfxUniform)
EndComponent()

void OnUniformRemoved(Entity entity) {
    auto data = GetBgfxUniformData(entity);

    if(bgfx::isValid(data->handle)) {
        bgfx::destroy(data->handle);
        data->handle = BGFX_INVALID_HANDLE;
    }
}

LocalFunction(OnChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_BgfxUniform())) {
        GetBgfxUniformData(entity)->invalidated = true;
    }
}

u16 GetBgfxUniformHandle(Entity entity) {
    auto data = GetBgfxUniformData(entity);

    if(data->invalidated) {
        // Eventually free old buffers
        if(bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }

        // Create or update unifom
        bgfx::UniformType::Enum type;

        if(GetUniformType(entity) == TypeOf_v4f()) {
            type = bgfx::UniformType::Vec4;
        } else if(GetUniformType(entity) == TypeOf_m3x3f()) {
            type = bgfx::UniformType::Mat3;
        } else if(GetUniformType(entity) == TypeOf_m4x4f()) {
            type = bgfx::UniformType::Mat4;
        } else if(GetUniformType(entity) == TypeOf_Entity()) {
            type = bgfx::UniformType::Int1;
        } else {
            Log(LogChannel_Core, LogSeverity_Error, "Unsupported uniform type: %s", GetTypeName(GetUniformType(entity)));
            return bgfx::kInvalidHandle;
        }

        auto name = GetUniformName(entity);
        auto arrayCount = std::max((u32)1, GetUniformArrayCount(entity));
        data->handle = bgfx::createUniform(name, type, arrayCount);

        data->invalidated = false;
    }
    return data->handle.idx;
}

DefineService(BgfxUniform)
        RegisterSubscription(BgfxUniformRemoved, OnUniformRemoved, 0)
        RegisterSubscription(UniformChanged, OnChanged, 0)
EndService()
