//
// Created by Kim Johannsen on 16/01/2018.
//

#include "BgfxUniform.h"
#include "BgfxResource.h"
#include <bgfx/bgfx.h>
#include <Rendering/Uniform.h>
#include <Rendering/RenderContext.h>
#include <Rendering/RenderingModule.h>
#include <Foundation/AppLoop.h>
#include <Core/Debug.h>
#include <Core/Identification.h>
#include <Core/Algorithms.h>

struct BgfxUniform {
};

static void OnBgfxResourceChanged(Entity entity, const BgfxResource& oldData, const BgfxResource& newData) {
    if(oldData.BgfxResourceHandle != bgfx::kInvalidHandle && newData.BgfxResourceHandle != oldData.BgfxResourceHandle) {
        if(HasComponent(entity, ComponentOf_BgfxUniform())) {
            bgfx::UniformHandle handle = { oldData.BgfxResourceHandle };

            if(bgfx::isValid(handle)) {
                bgfx::destroy(handle);
            }
        }
    }
}

static eastl::set<Entity> invalidatedUniforms;

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    for(auto& entity : invalidatedUniforms) {
        // Create or update unifom
        bgfx::UniformType::Enum type;
        auto uniformData = GetUniform(entity);

        auto property = uniformData.UniformEntityProperty;

        auto propertyType = GetProperty(property).PropertyType;
		
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

        auto name = uniformData.UniformIdentifier;
        auto arrayCount = Max((u32)1, uniformData.UniformArrayCount);
        SetBgfxResource(entity, {bgfx::createUniform(name, type, arrayCount).idx});
    }

    invalidatedUniforms.clear();
}

static void OnUniformChanged(Entity entity, const Uniform& oldData, const Uniform& newData) {
	invalidatedUniforms.insert(entity);
}

BeginUnit(BgfxUniform)
    BeginComponent(BgfxUniform)
        RegisterBase(BgfxResource)
    EndComponent()

    RegisterSystem(OnUniformChanged, ComponentOf_Uniform())
    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_ResourceSubmission)
EndUnit()
