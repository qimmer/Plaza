//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Core/Hierarchy.h>
#include "Material.h"
#include <Core/Enum.h>

struct Material {
    Material() : MaterialDepthTest(RenderState_STATE_DEPTH_TEST_LESS),
                 MaterialWriteMask(RenderState_STATE_RGB_WRITE | RenderState_STATE_ALPHA_WRITE |RenderState_STATE_DEPTH_WRITE),
                 MaterialMultisampleMode(RenderState_STATE_MSAA),
                 MaterialBlendMode(RenderState_STATE_BLEND_NONE),
				 MaterialProgram(0) {}

    Entity MaterialProgram;
    u64 MaterialDepthTest, MaterialWriteMask, MaterialMultisampleMode, MaterialBlendMode;
};

DefineEnum(DepthTest, false)
    DefineFlag(RenderState_STATE_DEPTH_TEST_NONE)
    DefineFlag(RenderState_STATE_DEPTH_TEST_LESS)
    DefineFlag(RenderState_STATE_DEPTH_TEST_LEQUAL)
    DefineFlag(RenderState_STATE_DEPTH_TEST_EQUAL)
    DefineFlag(RenderState_STATE_DEPTH_TEST_GEQUAL)
    DefineFlag(RenderState_STATE_DEPTH_TEST_GREATER)
    DefineFlag(RenderState_STATE_DEPTH_TEST_NOTEQUAL)
    DefineFlag(RenderState_STATE_DEPTH_TEST_NEVER)
    DefineFlag(RenderState_STATE_DEPTH_TEST_ALWAYS)
EndEnum()

DefineEnum(WriteMask, true)
    DefineFlag(RenderState_STATE_RGB_WRITE)
    DefineFlag(RenderState_STATE_ALPHA_WRITE)
    DefineFlag(RenderState_STATE_DEPTH_WRITE)
EndEnum()

DefineEnum(MultisampleMode, false)
    DefineFlag(RenderState_STATE_MSAA)
    DefineFlag(RenderState_STATE_LINEAA)
    DefineFlag(RenderState_STATE_CONSERVATIVE_RASTER)
EndEnum()

DefineEnum(BlendMode, false)
    DefineFlag(RenderState_STATE_BLEND_NONE)
    DefineFlag(RenderState_STATE_BLEND_NORMAL)
    DefineFlag(RenderState_STATE_BLEND_ALPHA)
    DefineFlag(RenderState_STATE_BLEND_ADD)
    DefineFlag(RenderState_STATE_BLEND_MULTIPLY)
    DefineFlag(RenderState_STATE_BLEND_DARKEN)
    DefineFlag(RenderState_STATE_BLEND_LIGHTEN)
    DefineFlag(RenderState_STATE_BLEND_SCREEN)
    DefineFlag(RenderState_STATE_BLEND_LINEAR_BURN)
EndEnum()

DefineComponent(Material)
    Dependency(Hierarchy)
    DefineProperty(Entity, MaterialProgram)
    DefinePropertyEnum(u64, MaterialDepthTest, DepthTest)
    DefinePropertyEnum(u64, MaterialWriteMask, WriteMask)
    DefinePropertyEnum(u64, MaterialMultisampleMode, MultisampleMode)
    DefinePropertyEnum(u64, MaterialBlendMode, BlendMode)
EndComponent()

DefineComponentProperty(Material, Entity, MaterialProgram)
DefineComponentProperty(Material, u64, MaterialDepthTest)
DefineComponentProperty(Material, u64, MaterialWriteMask)
DefineComponentProperty(Material, u64, MaterialMultisampleMode)
DefineComponentProperty(Material, u64, MaterialBlendMode)

