//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Mesh.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

struct Mesh {
    Entity MeshVertexBuffer, MeshIndexBuffer;
    u32 MeshStartVertex, MeshStartIndex, MeshNumIndices, MeshNumVertices, MeshPrimitiveType;
};

DefineComponent(Mesh)
    DefineProperty(Entity, MeshVertexBuffer)
    DefineProperty(Entity, MeshIndexBuffer)
    DefineProperty(u32, MeshStartVertex)
    DefineProperty(u32, MeshStartIndex)
    DefineProperty(u32, MeshNumIndices)
    DefineProperty(u32, MeshNumVertices)
    DefineProperty(u32, MeshPrimitiveType)
EndComponent()

DefineComponentProperty(Mesh, Entity, MeshVertexBuffer)
DefineComponentProperty(Mesh, Entity, MeshIndexBuffer)
DefineComponentProperty(Mesh, u32, MeshStartVertex)
DefineComponentProperty(Mesh, u32, MeshStartIndex)
DefineComponentProperty(Mesh, u32, MeshNumIndices)
DefineComponentProperty(Mesh, u32, MeshNumVertices)
DefineComponentProperty(Mesh, u32, MeshPrimitiveType)
