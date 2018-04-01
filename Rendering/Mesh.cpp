//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Mesh.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Material.h"

DefineEnum(CullMode, false)
        DefineFlag(RenderState_STATE_CULL_NONE)
        DefineFlag(RenderState_STATE_CULL_CW)
        DefineFlag(RenderState_STATE_CULL_CCW)
EndEnum()

DefineEnum(PrimitiveType, false)
        DefineFlag(RenderState_STATE_PT_TRIANGLELIST)
        DefineFlag(RenderState_STATE_PT_TRISTRIP)
        DefineFlag(RenderState_STATE_PT_LINES)
        DefineFlag(RenderState_STATE_PT_LINESTRIP)
        DefineFlag(RenderState_STATE_PT_POINTS)
EndEnum()

struct Mesh {
    Mesh() : MeshCullMode(RenderState_STATE_CULL_CCW) {}

    Entity MeshVertexBuffer, MeshIndexBuffer;
    u32 MeshStartVertex, MeshStartIndex, MeshNumIndices, MeshNumVertices;
    u64 MeshPrimitiveType, MeshCullMode;
};

DefineComponent(Mesh)
    DefinePropertyReactive(Entity, MeshVertexBuffer)
    DefinePropertyReactive(Entity, MeshIndexBuffer)
    DefinePropertyReactive(u32, MeshStartVertex)
    DefinePropertyReactive(u32, MeshStartIndex)
    DefinePropertyReactive(u32, MeshNumIndices)
    DefinePropertyReactive(u32, MeshNumVertices)
    DefinePropertyReactiveEnum(u64, MeshPrimitiveType, PrimitiveType)
    DefinePropertyReactiveEnum(u64, MeshCullMode, CullMode)
EndComponent()

DefineComponentPropertyReactive(Mesh, Entity, MeshVertexBuffer)
DefineComponentPropertyReactive(Mesh, Entity, MeshIndexBuffer)
DefineComponentPropertyReactive(Mesh, u32, MeshStartVertex)
DefineComponentPropertyReactive(Mesh, u32, MeshStartIndex)
DefineComponentPropertyReactive(Mesh, u32, MeshNumIndices)
DefineComponentPropertyReactive(Mesh, u32, MeshNumVertices)
DefineComponentPropertyReactive(Mesh, u64, MeshPrimitiveType)
DefineComponentPropertyReactive(Mesh, u64, MeshCullMode)
