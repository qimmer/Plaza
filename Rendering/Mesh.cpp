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
    DefineProperty(Entity, MeshVertexBuffer)
    DefineProperty(Entity, MeshIndexBuffer)
    DefineProperty(u32, MeshStartVertex)
    DefineProperty(u32, MeshStartIndex)
    DefineProperty(u32, MeshNumIndices)
    DefineProperty(u32, MeshNumVertices)
    DefinePropertyEnum(u64, MeshPrimitiveType, PrimitiveType)
    DefinePropertyEnum(u64, MeshCullMode, CullMode)
EndComponent()

DefineComponentProperty(Mesh, Entity, MeshVertexBuffer)
DefineComponentProperty(Mesh, Entity, MeshIndexBuffer)
DefineComponentProperty(Mesh, u32, MeshStartVertex)
DefineComponentProperty(Mesh, u32, MeshStartIndex)
DefineComponentProperty(Mesh, u32, MeshNumIndices)
DefineComponentProperty(Mesh, u32, MeshNumVertices)
DefineComponentProperty(Mesh, u64, MeshPrimitiveType)
DefineComponentProperty(Mesh, u64, MeshCullMode)
