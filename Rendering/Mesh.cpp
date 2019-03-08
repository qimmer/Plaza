//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Foundation/Stream.h>
#include "Mesh.h"
#include "Material.h"

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
        RegisterFlag(PrimitiveType_TRIANGLELIST)
        RegisterFlag(PrimitiveType_TRISTRIP)
        RegisterFlag(PrimitiveType_LINES)
        RegisterFlag(PrimitiveType_LINESTRIP)
        RegisterFlag(PrimitiveType_POINTS)
    EndEnum()
    
    BeginEnum(VertexAttributeUsage, false)
        RegisterFlag(VertexAttributeUsage_Position)
        RegisterFlag(VertexAttributeUsage_Normal)
        RegisterFlag(VertexAttributeUsage_Tangent)
        RegisterFlag(VertexAttributeUsage_Bitangent)
        RegisterFlag(VertexAttributeUsage_Color0)
        RegisterFlag(VertexAttributeUsage_Color1)
        RegisterFlag(VertexAttributeUsage_Color2)
        RegisterFlag(VertexAttributeUsage_Color3)
        RegisterFlag(VertexAttributeUsage_Indices)
        RegisterFlag(VertexAttributeUsage_Weight)
        RegisterFlag(VertexAttributeUsage_TexCoord0)
        RegisterFlag(VertexAttributeUsage_TexCoord1)
        RegisterFlag(VertexAttributeUsage_TexCoord2)
        RegisterFlag(VertexAttributeUsage_TexCoord3)
        RegisterFlag(VertexAttributeUsage_TexCoord4)
        RegisterFlag(VertexAttributeUsage_TexCoord5)
        RegisterFlag(VertexAttributeUsage_TexCoord6)
        RegisterFlag(VertexAttributeUsage_TexCoord7)
    EndEnum()

    BeginComponent(VertexDeclaration)
        RegisterArrayProperty(VertexAttribute, VertexDeclarationAttributes)
    EndComponent()

    BeginComponent(VertexAttribute)
        RegisterProperty(Type, VertexAttributeType)
        RegisterPropertyEnum(u8, VertexAttributeUsage, VertexAttributeUsage)
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
        RegisterProperty(bool, IndexBufferDynamic)
        RegisterProperty(bool, IndexBufferLong)
    EndComponent()

    BeginComponent(Mesh)
        RegisterChildProperty(VertexBuffer, MeshVertexBuffer)
        RegisterChildProperty(IndexBuffer, MeshIndexBuffer)
        RegisterArrayProperty(SubMesh, MeshSubMeshes)
        RegisterProperty(v3f, MeshAABBMin)
        RegisterProperty(v3f, MeshAABBMax)
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
