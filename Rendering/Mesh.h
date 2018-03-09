//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_MESH_H
#define PLAZA_MESH_H

#include <Core/Entity.h>

DeclareComponent(Mesh)

DeclareComponentProperty(Mesh, Entity, MeshVertexBuffer)
DeclareComponentProperty(Mesh, Entity, MeshIndexBuffer)
DeclareComponentProperty(Mesh, u32, MeshStartVertex)
DeclareComponentProperty(Mesh, u32, MeshStartIndex)
DeclareComponentProperty(Mesh, u32, MeshNumIndices)
DeclareComponentProperty(Mesh, u32, MeshNumVertices)
DeclareComponentProperty(Mesh, u32, MeshPrimitiveType)

#define PrimitiveType_TRIANGLES             UINT64_C(0x0000000000000000) //!< Triangle list.
#define PrimitiveType_TRISTRIP             UINT64_C(0x0001000000000000) //!< Tristrip.
#define PrimitiveType_LINES                UINT64_C(0x0002000000000000) //!< Lines.
#define PrimitiveType_LINESTRIP            UINT64_C(0x0003000000000000) //!< Line strip.
#define PrimitiveType_POINTS               UINT64_C(0x0004000000000000) //!< Points.
#define PrimitiveType_SHIFT                48                           //!< Primitive type bit shift.
#define PrimitiveType_MASK                 UINT64_C(0x0007000000000000) //!< Primitive type bit mask.

#endif //PLAZA_MESH_H
