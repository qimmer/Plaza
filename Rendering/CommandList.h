//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_COMMANDLIST_H
#define PLAZA_COMMANDLIST_H

#include <Core/Entity.h>


    DeclareComponent(CommandList)

    DeclareComponentProperty(CommandList, Entity, CommandListRenderTarget)
    DeclareComponentProperty(CommandList, u64, CommandListViewRenderState)
    DeclareComponentProperty(CommandList, m4x4f, CommandListViewMatrix)
    DeclareComponentProperty(CommandList, m4x4f, CommandListProjectionMatrix)
    DeclareComponentProperty(CommandList, rgba8, CommandListClearColor)
    DeclareComponentProperty(CommandList, float, CommandListClearDepth)
    DeclareComponentProperty(CommandList, u8, CommandListClearStencil)
    DeclareComponentProperty(CommandList, u8, CommandListClearTargets)
    DeclareComponentProperty(CommandList, bool, CommandListForceOrder)
    DeclareComponentProperty(CommandList, v4i, CommandListViewport)


#define ClearTarget_None 0
#define ClearTarget_Color 1
#define ClearTarget_Depth 2
#define ClearTarget_Stencil 4

#endif //PLAZA_COMMANDLIST_H
