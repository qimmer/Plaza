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

    DefineComponent(CommandList)
        DefineProperty(u8, CommandListLayer)
        DefineProperty(Entity, CommandListRenderTarget)
        DefineProperty(u64, CommandListViewRenderState)
        DefineProperty(rgba8, CommandListClearColor)
        DefineProperty(float, CommandListClearDepth)
        DefineProperty(u8, CommandListClearStencil)
        DefineProperty(bool, CommandListForceOrder)
        DefineProperty(v4i, CommandListViewport)
        DefineProperty(m4x4f, CommandListViewMatrix)
        DefineProperty(m4x4f, CommandListProjectionMatrix)
    EndComponent()

    DefineComponentProperty(CommandList, u8, CommandListLayer)
    DefineComponentProperty(CommandList, Entity, CommandListRenderTarget)
    DefineComponentProperty(CommandList, u64, CommandListViewRenderState)
    DefineComponentProperty(CommandList, m4x4f, CommandListViewMatrix)
    DefineComponentProperty(CommandList, m4x4f, CommandListProjectionMatrix)
    DefineComponentProperty(CommandList, rgba8, CommandListClearColor)
    DefineComponentProperty(CommandList, float, CommandListClearDepth)
    DefineComponentProperty(CommandList, u8, CommandListClearStencil)
    DefineComponentProperty(CommandList, u8, CommandListClearTargets)
    DefineComponentProperty(CommandList, bool, CommandListForceOrder)
    DefineComponentProperty(CommandList, v4i, CommandListViewport)
