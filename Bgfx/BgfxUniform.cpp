//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxUniform.h"
#include <bgfx/bgfx.h>
#include <Rendering/Uniform.h>
#include <Foundation/Invalidation.h>


    struct BgfxUniform {
        BgfxUniform() :
                handle(BGFX_INVALID_HANDLE) {}

        bgfx::UniformHandle handle;
    };

    DefineComponent(BgfxUniform)
    EndComponent()

    DefineService(BgfxUniform)
    EndService()

    void UpdateBgfxUniform(Entity entity) {
        if(!HasBgfxUniform(entity) || GetUniformType(entity) == 0)
        {
            return;
        }

        auto data = GetBgfxUniform(entity);

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
            return;
        }

        data->handle = bgfx::createUniform(GetUniformName(entity), type, std::max((u32)1, GetUniformArrayCount(entity)));
    }

    void OnUniformRemoved(Entity entity) {
        auto data = GetBgfxUniform(entity);

        if(bgfx::isValid(data->handle)) {
            bgfx::destroy(data->handle);
            data->handle = BGFX_INVALID_HANDLE;
        }
    }

    static bool ServiceStart() {
        SubscribeBgfxUniformRemoved(OnUniformRemoved);

        return true;
    }

    static bool ServiceStop() {
        UnsubscribeBgfxUniformRemoved(OnUniformRemoved);

        return true;
    }

    u16 GetBgfxUniformHandle(Entity entity) {
        return GetBgfxUniform(entity)->handle.idx;
    }

