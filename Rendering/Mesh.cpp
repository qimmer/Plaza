//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Foundation/Stream.h>
#include "Mesh.h"
#include "Material.h"

struct VertexBuffer {
    Entity VertexBufferDeclaration;
    bool VertexBufferDynamic;
};

struct VertexDeclaration {
    Vector(VertexDeclarationAttributes, Entity, 16)
};

struct VertexAttribute {
    Type VertexAttributeType;
    u8 VertexAttributeUsage;
    bool VertexAttributeNormalize, VertexAttributeAsInt;
};

struct IndexBuffer {
    bool IndexBufferLong, IndexBufferDynamic;
};

struct Mesh {
    Entity MeshVertexBuffer, MeshIndexBuffer;
    Vector(MeshSubMeshes, Entity, 16)
};

struct SubMesh {
    u32 SubMeshStartVertex, SubMeshStartIndex, SubMeshNumIndices, SubMeshNumVertices;
    u64 SubMeshPrimitiveType, SubMeshCullMode;
};

API_EXPORT u32 GetVertexStride(Entity vertexDeclaration) {
    u32 stride = 0;
    for_children(attribute, VertexDeclarationAttributes, vertexDeclaration) {
        stride += GetTypeSize(GetVertexAttributeType(attribute));
    }
    return stride;
}

BeginUnit(Mesh)
    BeginEnum(CullMode, false)
        RegisterFlag(RenderState_STATE_CULL_NONE)
        RegisterFlag(RenderState_STATE_CULL_CW)
        RegisterFlag(RenderState_STATE_CULL_CCW)
    EndEnum()

    BeginEnum(PrimitiveType, false)
        RegisterFlag(RenderState_STATE_PT_TRIANGLELIST)
        RegisterFlag(RenderState_STATE_PT_TRISTRIP)
        RegisterFlag(RenderState_STATE_PT_LINES)
        RegisterFlag(RenderState_STATE_PT_LINESTRIP)
        RegisterFlag(RenderState_STATE_PT_POINTS)
    EndEnum()

    BeginComponent(VertexDeclaration)
        RegisterArrayProperty(VertexAttribute, VertexDeclarationAttributes)
    EndComponent()

    BeginComponent(VertexAttribute)
        RegisterProperty(Type, VertexAttributeType)
        RegisterProperty(u8, VertexAttributeUsage)
        RegisterProperty(bool, VertexAttributeNormalize)
        RegisterProperty(bool, VertexAttributeAsInt)
    EndComponent()

    BeginComponent(VertexBuffer)
        RegisterBase(Stream)
        RegisterReferenceProperty(VertexDeclaration, VertexBufferDeclaration)
        RegisterProperty(bool, VertexBufferDynamic)
    EndComponent()

    BeginComponent(IndexBuffer)
        RegisterBase(Stream)
    EndComponent()

    BeginComponent(Mesh)
        RegisterChildProperty(VertexBuffer, MeshVertexBuffer)
        RegisterChildProperty(IndexBuffer, MeshIndexBuffer)
        RegisterArrayProperty(SubMesh, MeshSubMeshes)
    EndComponent()

    BeginComponent(SubMesh)
        RegisterProperty(u32, SubMeshStartVertex)
        RegisterProperty(u32, SubMeshStartIndex)
        RegisterProperty(u32, SubMeshNumIndices)
        RegisterProperty(u32, SubMeshNumVertices)
        RegisterPropertyEnum(u64, SubMeshPrimitiveType, PrimitiveType)
        RegisterPropertyEnum(u64, SubMeshCullMode, CullMode)
    EndComponent()
EndUnit()
