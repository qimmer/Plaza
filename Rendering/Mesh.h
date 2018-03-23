//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_MESH_H
#define PLAZA_MESH_H

#include <Core/Entity.h>

#define RenderState_STATE_PT_TRIANGLELIST         UINT64_C(0x0000000000000000)
#define RenderState_STATE_PT_TRISTRIP             UINT64_C(0x0001000000000000) //!< Tristrip.
#define RenderState_STATE_PT_LINES                UINT64_C(0x0002000000000000) //!< Lines.
#define RenderState_STATE_PT_LINESTRIP            UINT64_C(0x0003000000000000) //!< Line strip.
#define RenderState_STATE_PT_POINTS               UINT64_C(0x0004000000000000) //!< Points.

#define RenderState_STATE_CULL_NONE               UINT64_C(0x0000000000000000)
#define RenderState_STATE_CULL_CW                 UINT64_C(0x0000001000000000) //!< Cull clockwise triangles.
#define RenderState_STATE_CULL_CCW                UINT64_C(0x0000002000000000) //!< Cull counter-clockwise triangles.

DeclareComponent(Mesh)

DeclareComponentProperty(Mesh, Entity, MeshVertexBuffer)
DeclareComponentProperty(Mesh, Entity, MeshIndexBuffer)
DeclareComponentProperty(Mesh, u32, MeshStartVertex)
DeclareComponentProperty(Mesh, u32, MeshStartIndex)
DeclareComponentProperty(Mesh, u32, MeshNumIndices)
DeclareComponentProperty(Mesh, u32, MeshNumVertices)
DeclareComponentProperty(Mesh, u64, MeshPrimitiveType)
DeclareComponentProperty(Mesh, u64, MeshCullMode)

#endif //PLAZA_MESH_H
