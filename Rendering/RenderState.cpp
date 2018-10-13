//
// Created by Kim on 20-09-2018.
//

#include "RenderState.h"


struct RenderState {
    u64 RenderStateDepthTest, RenderStateWriteMask, RenderStateMultisampleMode, RenderStateBlendMode;
};

LocalFunction(OnAdded, void, Entity component, Entity entity) {
    SetRenderStateDepthTest(entity, RenderState_STATE_DEPTH_TEST_LEQUAL);
    SetRenderStateWriteMask(entity, RenderState_STATE_RGB_WRITE | RenderState_STATE_ALPHA_WRITE | RenderState_STATE_DEPTH_WRITE);
}

BeginUnit(RenderState)
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
        RegisterFlag(RenderState_STATE_R_WRITE)
        RegisterFlag(RenderState_STATE_G_WRITE)
        RegisterFlag(RenderState_STATE_B_WRITE)
        RegisterFlag(RenderState_STATE_ALPHA_WRITE)
        RegisterFlag(RenderState_STATE_DEPTH_WRITE)
    EndEnum()

    BeginEnum(MultisampleMode, false)
        RegisterFlag(RenderState_STATE_NONE)
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

    BeginComponent(RenderState)
        RegisterPropertyEnum(u64, RenderStateDepthTest, DepthTest)
        RegisterPropertyEnum(u64, RenderStateWriteMask, WriteMask)
        RegisterPropertyEnum(u64, RenderStateMultisampleMode, MultisampleMode)
        RegisterPropertyEnum(u64, RenderStateBlendMode, BlendMode)
    EndComponent()

    RegisterSubscription(EntityComponentAdded, OnAdded, ComponentOf_RenderState())
EndUnit()