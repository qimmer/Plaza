//
// Created by Kim on 04-10-2018.
//

#include <Foundation/Stream.h>
#include <Foundation/Invalidation.h>
#include "MeshBuilder.h"
#include "Mesh.h"

struct MeshBuilderVertex {
    v3f MeshBuilderVertexPosition, MeshBuilderVertexNormal;
    v2f MeshBuilderVertexTexCoord0;
    rgba8 MeshBuilderVertexColor0;
};

struct MeshBuilderIndex {
    u32 MeshBuilderIndexVertexIndex;
};

struct MeshBuilder {
    Vector(MeshBuilderVertices, Entity, 128);
    Vector(MeshBuilderIndices, Entity, 128);
};

static void BuildVertexBuffer(Entity mesh) {
    auto vb = GetMeshVertexBuffer(mesh);
    auto decl = GetVertexBufferDeclaration(vb);
    auto attribs = GetVertexDeclarationAttributes(decl);
    auto numAttribs = GetNumVertexDeclarationAttributes(decl);
    auto numVertices = GetNumMeshBuilderVertices(mesh);

    auto vertexStride = 0;
    for(auto i = 0; i < numAttribs; ++i) {
        vertexStride += GetTypeSize(GetVertexAttributeType(attribs[i]));
    }

    auto vertexBufferSize = vertexStride * numVertices;
    auto vertexData = (char*)malloc(vertexBufferSize);
    auto vertices = GetMeshBuilderVertices(mesh);

    auto attribOffset = 0;
    for(auto i = 0; i < numAttribs; ++i) {
        auto vertexOffset = 0;

        auto attribUsage = GetVertexAttributeUsage(attribs[i]);
        auto attribType = GetVertexAttributeType(attribs[i]);
        auto attribSize = GetTypeSize(attribType);

        for(auto v = 0; v < numVertices; ++v) {
            auto vertex = GetMeshBuilderVertexData(vertices[v]);
            auto offset = vertexOffset + attribOffset;

            Variant value;
            switch(attribUsage) {
                case VertexAttributeUsage_Position:
                    value.type = TypeOf_v3f;
                    value.as_v3f = vertex->MeshBuilderVertexPosition;
                    break;
                case VertexAttributeUsage_Normal:
                    value.type = TypeOf_v3f;
                    value.as_v3f = vertex->MeshBuilderVertexNormal;
                    break;
                case VertexAttributeUsage_TexCoord0:
                    value.type = TypeOf_v2f;
                    value.as_v2f = vertex->MeshBuilderVertexTexCoord0;
                    break;
                case VertexAttributeUsage_Color0:
                    value.type = TypeOf_rgba8;
                    value.as_rgba8 = vertex->MeshBuilderVertexColor0;
                    break;
            }

            value = Cast(value, attribType);

            memcpy(vertexData + offset, &value.data, attribSize);

            vertexOffset += vertexStride;
        }

        attribOffset += GetTypeSize(attribType);
    }

    char path[1024];
    snprintf(path, sizeof(path), "memory://%llu.vtx", vb);
    SetStreamPath(vb, path);
    if(StreamOpen(vb, StreamMode_Write)) {
        StreamWrite(vb, vertexBufferSize, vertexData);
        StreamClose(vb);
    }

    free(vertexData);
}

static void BuildIndexBuffer(Entity mesh) {
    auto ib = GetMeshIndexBuffer(mesh);

    auto numIndices = GetNumMeshBuilderIndices(mesh);
    auto isIndexLong = GetIndexBufferLong(ib);

    auto indexBufferSize = (isIndexLong ? 4 : 2) * numIndices;
    auto indexData = (char*)malloc(indexBufferSize);

    auto indices = GetMeshBuilderIndices(mesh);
    auto offset = 0;
    if(isIndexLong) {
        for(auto i = 0; i < numIndices; ++i) {
            auto index = GetMeshBuilderIndexData(indices[i]);

            *(u32*)(indexData + offset) = index->MeshBuilderIndexVertexIndex;
            offset += 4;
        }
    } else {
        for(auto i = 0; i < numIndices; ++i) {
            auto index = GetMeshBuilderIndexData(indices[i]);

            *(u16*)(indexData + offset) = (u16)index->MeshBuilderIndexVertexIndex;
            offset += 2;
        }
    }

    char path[1024];
    snprintf(path, sizeof(path), "memory://%llu.idx", ib);
    SetStreamPath(ib, path);
    if(StreamOpen(ib, StreamMode_Write)) {
        StreamWrite(ib, indexBufferSize, indexData);
        StreamClose(ib);
    }

    free(indexData);
}

static void BuildMesh(Entity mesh) {
    BuildVertexBuffer(mesh);
    BuildIndexBuffer(mesh);
}

LocalFunction(OnMeshBuilderChildChanged, void, Entity vertex) {
    auto mesh = GetOwner(vertex);
    Invalidate(mesh);
}

LocalFunction(OnValidation, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_MeshBuilder())) {
        BuildMesh(entity);
    }

    if(HasComponent(entity, ComponentOf_VertexDeclaration())) {
        for_entity(mb, data, MeshBuilder) {
            auto vb = GetMeshVertexBuffer(mb);
            auto vd = GetVertexBufferDeclaration(vb);

            if(vd == entity) {
                Invalidate(mb);
            }
        }
    }
}

BeginUnit(MeshBuilder)
    BeginComponent(MeshBuilderVertex)
        RegisterProperty(v3f, MeshBuilderVertexPosition)
        RegisterProperty(v3f, MeshBuilderVertexNormal)
        RegisterProperty(v2f, MeshBuilderVertexTexCoord0)
        RegisterProperty(rgba8, MeshBuilderVertexColor0)
    EndComponent()

    BeginComponent(MeshBuilderIndex)
        RegisterProperty(u32, MeshBuilderIndexVertexIndex)
    EndComponent()

    BeginComponent(MeshBuilder)
        RegisterBase(Mesh)
        RegisterArrayProperty(MeshBuilderVertex, MeshBuilderVertices)
        RegisterArrayProperty(MeshBuilderIndex, MeshBuilderIndices)
    EndComponent()

    RegisterSubscription(MeshBuilderVertexPositionChanged, OnMeshBuilderChildChanged, 0)
    RegisterSubscription(MeshBuilderVertexNormalChanged, OnMeshBuilderChildChanged, 0)
    RegisterSubscription(MeshBuilderVertexTexCoord0Changed, OnMeshBuilderChildChanged, 0)
    RegisterSubscription(MeshBuilderVertexColor0Changed, OnMeshBuilderChildChanged, 0)
    RegisterSubscription(MeshBuilderVertexColor0Changed, OnMeshBuilderChildChanged, 0)
    RegisterSubscription(MeshBuilderIndexVertexIndexChanged, OnMeshBuilderChildChanged, 0)
    RegisterSubscription(IndexBufferLongChanged, OnMeshBuilderChildChanged, 0)
    RegisterSubscription(MeshBuilderVerticesChanged, Invalidate, 0)
    RegisterSubscription(MeshBuilderIndicesChanged, Invalidate, 0)
    RegisterSubscription(Validate, OnValidation, 0)
EndUnit()