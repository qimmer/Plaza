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

BeginUnit(Mesh)
    BeginComponent(Mesh)
    RegisterProperty(Entity, MeshVertexBuffer)
    RegisterProperty(Entity, MeshIndexBuffer)
    RegisterProperty(u32, MeshStartVertex)
    RegisterProperty(u32, MeshStartIndex)
    RegisterProperty(u32, MeshNumIndices)
    RegisterProperty(u32, MeshNumVertices)
    DefinePropertyReactiveEnum(u64, MeshPrimitiveType, PrimitiveType)
    DefinePropertyReactiveEnum(u64, MeshCullMode, CullMode)
EndComponent()

RegisterProperty(Entity, MeshVertexBuffer)
RegisterProperty(Entity, MeshIndexBuffer)
RegisterProperty(u32, MeshStartVertex)
RegisterProperty(u32, MeshStartIndex)
RegisterProperty(u32, MeshNumIndices)
RegisterProperty(u32, MeshNumVertices)
RegisterProperty(u64, MeshPrimitiveType)
RegisterProperty(u64, MeshCullMode)
