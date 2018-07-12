//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Core/Node.h>
#include "Material.h"
#include <Core/Enum.h>

struct Material {
    Material() : MaterialDepthTest(RenderState_STATE_DEPTH_TEST_LESS),
                 MaterialWriteMask(RenderState_STATE_RGB_WRITE | RenderState_STATE_ALPHA_WRITE |RenderState_STATE_DEPTH_WRITE),
                 MaterialMultisampleMode(RenderState_STATE_MSAA),
                 MaterialBlendMode(RenderState_STATE_BLEND_NONE),
                 MaterialVertexShader(0), MaterialPixelShader(0) {}

    Entity MaterialVertexShader, MaterialPixelShader;
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

BeginUnit(Material)
    BeginComponent(Material)
    RegisterBase(Node)
    RegisterProperty(Entity, MaterialVertexShader)
    RegisterProperty(Entity, MaterialPixelShader)
    DefinePropertyReactiveEnum(u64, MaterialDepthTest, DepthTest)
    DefinePropertyReactiveEnum(u64, MaterialWriteMask, WriteMask)
    DefinePropertyReactiveEnum(u64, MaterialMultisampleMode, MultisampleMode)
    DefinePropertyReactiveEnum(u64, MaterialBlendMode, BlendMode)
EndComponent()

RegisterProperty(Entity, MaterialVertexShader)
RegisterProperty(Entity, MaterialPixelShader)
RegisterProperty(u64, MaterialDepthTest)
RegisterProperty(u64, MaterialWriteMask)
RegisterProperty(u64, MaterialMultisampleMode)
RegisterProperty(u64, MaterialBlendMode)

