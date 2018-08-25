//
// Created by Kim Johannsen on 14/01/2018.
//

#include "CommandList.h"
#include "RenderTarget.h"
#include "ShaderCache.h"
#include "Uniform.h"
#include "Batch.h"

struct CommandList {
    Vector(CommandListUniformStates, Entity, 16)
    Vector(CommandListBatches, Entity, 64)
    char CommandListShaderDefines[128];
    Entity CommandListRenderTarget;
    Entity CommandListShaderCache;
    u64 CommandListViewRenderState;
    m4x4f CommandListViewMatrix, CommandListProjectionMatrix;
    rgba8 CommandListClearColor;
    v4i CommandListViewport;
    float CommandListClearDepth;
    u8 CommandListClearStencil, CommandListClearTargets, CommandListLayer;
    bool CommandListForceOrder;
};

BeginUnit(CommandList)
    BeginComponent(CommandList)
        RegisterProperty(StringRef, CommandListShaderDefines)
        RegisterReferenceProperty(RenderTarget, CommandListRenderTarget)
        RegisterProperty(u64, CommandListViewRenderState)
        RegisterProperty(rgba8, CommandListClearColor)
        RegisterProperty(float, CommandListClearDepth)
        RegisterProperty(u8, CommandListClearStencil)
        RegisterProperty(bool, CommandListForceOrder)
        RegisterProperty(v4i, CommandListViewport)
        RegisterProperty(m4x4f, CommandListViewMatrix)
        RegisterProperty(m4x4f, CommandListProjectionMatrix)
        RegisterReferenceProperty(ShaderCache, CommandListShaderCache)
        RegisterArrayProperty(UniformState, CommandListUniformStates)
        RegisterArrayProperty(Batch, CommandListBatches)
    EndComponent()
EndUnit()
