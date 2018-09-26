//
// Created by Kim Johannsen on 14/01/2018.
//

#include "CommandList.h"
#include "RenderTarget.h"
#include "ShaderCache.h"
#include "Uniform.h"
#include "Batch.h"
#include "RenderState.h"

BeginUnit(CommandList)
    BeginComponent(CommandList)
        RegisterReferenceProperty(RenderTarget, CommandListRenderTarget)
        RegisterReferenceProperty(RenderState, CommandListRenderState)
        RegisterProperty(rgba8, CommandListClearColor)
        RegisterProperty(float, CommandListClearDepth)
        RegisterProperty(u8, CommandListClearStencil)
        RegisterProperty(bool, CommandListForceOrder)
        RegisterProperty(v4i, CommandListViewport)
        RegisterProperty(m4x4f, CommandListViewMatrix)
        RegisterProperty(m4x4f, CommandListProjectionMatrix)
        RegisterArrayProperty(UniformState, CommandListUniformStates)
        RegisterArrayProperty(Batch, CommandListBatches)
    EndComponent()
EndUnit()
