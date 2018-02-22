//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Foundation/Invalidation.h>
#include "Mesh.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"


    struct Mesh {
        Entity MeshVertexBuffer, MeshIndexBuffer;
        u32 MeshStartVertex, MeshStartIndex, MeshNumIndices, MeshNumVertices, MeshPrimitiveType;
    };

    DefineComponent(Mesh)
        Dependency(Invalidation)
    EndComponent()

    DefineService(MeshService)
    EndService()

    DefineComponentProperty(Mesh, Entity, MeshVertexBuffer)
    DefineComponentProperty(Mesh, Entity, MeshIndexBuffer)
    DefineComponentProperty(Mesh, u32, MeshStartVertex)
    DefineComponentProperty(Mesh, u32, MeshStartIndex)
    DefineComponentProperty(Mesh, u32, MeshNumIndices)
    DefineComponentProperty(Mesh, u32, MeshNumVertices)
    DefineComponentProperty(Mesh, u32, MeshPrimitiveType)

    static void OnInvalidated(Entity entity, bool before, bool after) {
        if(after) {
            if(HasVertexBuffer(entity) || HasIndexBuffer(entity)) {
                for(auto i = 0; i < GetNumMesh(); ++i) {
                    if(GetMeshByIndex(i)->MeshVertexBuffer == entity || GetMeshByIndex(i)->MeshIndexBuffer == entity) {
                        SetInvalidated(GetMeshEntity(i), true);
                        break;
                    }
                }
            }
        }
    }

    static bool ServiceStart() {
        SubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeInvalidatedChanged(OnInvalidated);
        return true;
    }

