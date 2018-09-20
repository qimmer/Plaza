//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_COMMANDLIST_H
#define PLAZA_COMMANDLIST_H

#include <Core/NativeUtils.h>

Unit(CommandList)
    Component(CommandList)
        ReferenceProperty(RenderTarget, CommandListRenderTarget)
        ReferenceProperty(ShaderCache, CommandListShaderCache)
        ReferenceProperty(RenderState, CommandListRenderState)
        Property(m4x4f, CommandListViewMatrix)
        Property(m4x4f, CommandListProjectionMatrix)
        Property(rgba8, CommandListClearColor)
        Property(float, CommandListClearDepth)
        Property(u8, CommandListClearStencil)
        Property(u8, CommandListClearTargets)
        Property(bool, CommandListForceOrder)
        Property(v4i, CommandListViewport)
        ArrayProperty(UniformState, CommandListUniformStates)
        ArrayProperty(Batch, CommandListBatches)

    Enum(ClearTarget)

#define ClearTarget_None 0
#define ClearTarget_Color 1
#define ClearTarget_Depth 2
#define ClearTarget_Stencil 4

#endif //PLAZA_COMMANDLIST_H
