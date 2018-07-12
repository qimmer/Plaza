//
// Created by Kim Johannsen on 14/01/2018.
//

#include "CommandList.h"


    struct CommandList {
        CommandList() :
                CommandListForceOrder(false),
                CommandListRenderTarget(0),
                CommandListViewRenderState(0),
                CommandListClearDepth(0.0f),
                CommandListClearStencil(0),
                CommandListClearColor({0, 0, 0, 1}),
                CommandListClearTargets(ClearTarget_Color | ClearTarget_Depth),
                CommandListViewport({0, 0, 0, 0}),
                CommandListViewMatrix(m4x4f_Identity),
                CommandListProjectionMatrix(m4x4f_Identity),
				CommandListLayer(0)
                {}

        Entity CommandListRenderTarget;
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
        RegisterProperty(u8, CommandListLayer))
        RegisterProperty(Entity, CommandListRenderTarget))
        RegisterProperty(u64, CommandListViewRenderState))
        RegisterProperty(rgba8, CommandListClearColor))
        RegisterProperty(float, CommandListClearDepth))
        RegisterProperty(u8, CommandListClearStencil))
        RegisterProperty(bool, CommandListForceOrder))
        RegisterProperty(v4i, CommandListViewport))
        RegisterProperty(m4x4f, CommandListViewMatrix))
        RegisterProperty(m4x4f, CommandListProjectionMatrix))
    EndComponent()

    RegisterProperty(u8, CommandListLayer)
    RegisterProperty(Entity, CommandListRenderTarget)
    RegisterProperty(u64, CommandListViewRenderState)
    RegisterProperty(m4x4f, CommandListViewMatrix)
    RegisterProperty(m4x4f, CommandListProjectionMatrix)
    RegisterProperty(rgba8, CommandListClearColor)
    RegisterProperty(float, CommandListClearDepth)
    RegisterProperty(u8, CommandListClearStencil)
    RegisterProperty(u8, CommandListClearTargets)
    RegisterProperty(bool, CommandListForceOrder)
    RegisterProperty(v4i, CommandListViewport)
