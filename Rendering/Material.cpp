//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Material.h"
#include "ShaderCache.h"
#include "Uniform.h"
#include <Core/Enum.h>

struct Material {
    Entity MaterialProgram;
    u64 MaterialDepthTest, MaterialWriteMask, MaterialMultisampleMode, MaterialBlendMode;
    Vector(MaterialUniformStates, Entity, 32)
};

BeginUnit(Material)
    BeginEnum(DepthTest, false)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_NONE)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_LESS)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_LEQUAL)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_EQUAL)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_GEQUAL)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_GREATER)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_NOTEQUAL)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_NEVER)
        RegisterFlag(RenderState_STATE_DEPTH_TEST_ALWAYS)
    EndEnum()

    BeginEnum(WriteMask, true)
        RegisterFlag(RenderState_STATE_RGB_WRITE)
        RegisterFlag(RenderState_STATE_ALPHA_WRITE)
        RegisterFlag(RenderState_STATE_DEPTH_WRITE)
    EndEnum()

    BeginEnum(MultisampleMode, false)
        RegisterFlag(RenderState_STATE_MSAA)
        RegisterFlag(RenderState_STATE_LINEAA)
        RegisterFlag(RenderState_STATE_CONSERVATIVE_RASTER)
    EndEnum()

    BeginEnum(BlendMode, false)
        RegisterFlag(RenderState_STATE_BLEND_NONE)
        RegisterFlag(RenderState_STATE_BLEND_NORMAL)
        RegisterFlag(RenderState_STATE_BLEND_ALPHA)
        RegisterFlag(RenderState_STATE_BLEND_ADD)
        RegisterFlag(RenderState_STATE_BLEND_MULTIPLY)
        RegisterFlag(RenderState_STATE_BLEND_DARKEN)
        RegisterFlag(RenderState_STATE_BLEND_LIGHTEN)
        RegisterFlag(RenderState_STATE_BLEND_SCREEN)
        RegisterFlag(RenderState_STATE_BLEND_LINEAR_BURN)
    EndEnum()

    BeginComponent(Material)
        RegisterProperty(Entity, MaterialProgram)
        RegisterPropertyEnum(u64, MaterialDepthTest, DepthTest)
        RegisterPropertyEnum(u64, MaterialWriteMask, WriteMask)
        RegisterPropertyEnum(u64, MaterialMultisampleMode, MultisampleMode)
        RegisterPropertyEnum(u64, MaterialBlendMode, BlendMode)
        RegisterArrayProperty(UniformState, MaterialUniformStates)
    EndComponent()
EndUnit()