//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_MESH_H
#define PLAZA_MESH_H

#include <Core/NativeUtils.h>

#define RenderState_STATE_PT_TRIANGLELIST         UINT64_C(0x0000000000000000)
#define RenderState_STATE_PT_TRISTRIP             UINT64_C(0x0001000000000000) //!< Tristrip.
#define RenderState_STATE_PT_LINES                UINT64_C(0x0002000000000000) //!< Lines.
#define RenderState_STATE_PT_LINESTRIP            UINT64_C(0x0003000000000000) //!< Line strip.
#define RenderState_STATE_PT_POINTS               UINT64_C(0x0004000000000000) //!< Points.

#define RenderState_STATE_CULL_NONE               UINT64_C(0x0000000000000000)
#define RenderState_STATE_CULL_CW                 UINT64_C(0x0000001000000000) //!< Cull clockwise triangles.
#define RenderState_STATE_CULL_CCW                UINT64_C(0x0000002000000000) //!< Cull counter-clockwise triangles.

enum
{
    VertexAttributeUsage_Position,
    VertexAttributeUsage_Normal,
    VertexAttributeUsage_Tangent,
    VertexAttributeUsage_Bitangent,
    VertexAttributeUsage_Color0,
    VertexAttributeUsage_Color1,
    VertexAttributeUsage_Color2,
    VertexAttributeUsage_Color3,
    VertexAttributeUsage_Indices,
    VertexAttributeUsage_Weight,
    VertexAttributeUsage_TexCoord0,
    VertexAttributeUsage_TexCoord1,
    VertexAttributeUsage_TexCoord2,
    VertexAttributeUsage_TexCoord3,
    VertexAttributeUsage_TexCoord4,
    VertexAttributeUsage_TexCoord5,
    VertexAttributeUsage_TexCoord6,
    VertexAttributeUsage_TexCoord7
};

u32 GetVertexStride(Entity vertexDeclaration);

Unit(Mesh)
    Enum(VertexAttributeUsage)

    Component(VertexAttribute)
        Property(Type, VertexAttributeType)
        Property(u8, VertexAttributeUsage)
        Property(bool, VertexAttributeNormalize)
        Property(bool, VertexAttributeAsInt)

    Component(VertexDeclaration)
        ArrayProperty(VertexAttribute, VertexDeclarationAttributes)

    Component(VertexBuffer)
        ReferenceProperty(VertexDeclaration, VertexBufferDeclaration)
        Property(bool, VertexBufferDynamic)

    Component(IndexBuffer)
        Property(bool, IndexBufferLong)
        Property(bool, IndexBufferDynamic)

    Component(Mesh)
        ChildProperty(VertexBuffer, MeshVertexBuffer)
        ChildProperty(IndexBuffer, MeshIndexBuffer)
        ArrayProperty(SubMesh, MeshSubMeshes)

    Component(SubMesh)
        Property(u32, SubMeshStartVertex)
        Property(u32, SubMeshStartIndex)
        Property(u32, SubMeshNumIndices)
        Property(u32, SubMeshNumVertices)
        Property(u64, SubMeshPrimitiveType)
        Property(u64, SubMeshCullMode)

    Enum(PrimitiveType)
    Enum(CullMode)

#endif //PLAZA_MESH_H
