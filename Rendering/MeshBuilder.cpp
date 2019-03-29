//
// Created by Kim on 04-10-2018.
//

#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include "MeshBuilder.h"
#include "Mesh.h"
#include <Rendering/RenderContext.h>

static eastl::set<Entity> invalidatedMeshes;

static void BuildVertexBuffer(Entity mesh) {
    auto vb = GetMeshVertexBuffer(mesh);
    auto decl = GetVertexBufferDeclaration(vb);
    auto& attribs = GetVertexDeclarationAttributes(decl);
    auto& vertices = GetMeshBuilderVertices(mesh);

    auto vertexStride = 0;
    for(auto i = 0; i < attribs.size(); ++i) {
        vertexStride += GetTypeSize(GetVertexAttributeType(attribs[i]));
    }

    auto vertexBufferSize = vertexStride * vertices.size();
    auto vertexData = (char*)malloc(vertexBufferSize);

    auto attribOffset = 0;
    for(auto i = 0; i < attribs.size(); ++i) {
        auto vertexOffset = 0;

        auto attribUsage = GetVertexAttributeUsage(attribs[i]);
        auto attribType = GetVertexAttributeType(attribs[i]);
        auto attribSize = GetTypeSize(attribType);

        for(auto v = 0; v < vertices.size(); ++v) {
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
                case VertexAttributeUsage_TexCoord1:
                    value.type = TypeOf_v2f;
                    value.as_v2f = vertex->MeshBuilderVertexTexCoord1;
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

    auto currentPath = GetStreamPath(vb);
    if(!currentPath || !strlen(currentPath)) {
        char path[1024];
        snprintf(path, sizeof(path), "memory://%s.vtb", GetIdentification(vb).Uuid);
        SetStreamPath(vb, path);
    }

    if(StreamOpen(vb, StreamMode_Write)) {
        StreamWrite(vb, vertexBufferSize, vertexData);
        StreamClose(vb);
    }

    free(vertexData);
}

static void BuildIndexBuffer(Entity mesh) {
    auto ib = GetMeshIndexBuffer(mesh);

    auto indices = GetMeshBuilderIndices(mesh);
    auto isIndexLong = GetIndexBufferLong(ib);

    auto indexBufferSize = (isIndexLong ? 4 : 2) * indices.size();
    auto indexData = (char*)malloc(indexBufferSize);

    auto offset = 0;
    if(isIndexLong) {
        for(auto i = 0; i < indices.size(); ++i) {
            auto index = GetMeshBuilderIndexData(indices[i]);

            *(u32*)(indexData + offset) = index->MeshBuilderIndexVertexIndex;
            offset += 4;
        }
    } else {
        for(auto i = 0; i < indices.size(); ++i) {
            auto index = GetMeshBuilderIndexData(indices[i]);

            *(u16*)(indexData + offset) = (u16)index->MeshBuilderIndexVertexIndex;
            offset += 2;
        }
    }

    auto currentPath = GetStreamPath(ib);
    if(!currentPath || !strlen(currentPath)) {
        char path[1024];
        snprintf(path, sizeof(path), "memory://%s.idb", GetIdentification(ib).Uuid);
        SetStreamPath(ib, path);
    }

    if(StreamOpen(ib, StreamMode_Write)) {
        StreamWrite(ib, indexBufferSize, indexData);
        StreamClose(ib);
    }

    free(indexData);
}

static void BuildMesh(Entity mesh) {
    auto data = GetMeshBuilderData(mesh);
    BuildVertexBuffer(mesh);
    BuildIndexBuffer(mesh);
}

LocalFunction(OnMeshValidation, void) {
    for(auto& mesh : invalidatedMeshes) {
        BuildMesh(mesh);
    }

    invalidatedMeshes.clear();
}

LocalFunction(OnVertexDeclarationChanged, void, Entity vertexDecl) {
    for_entity(mb, ComponentOf_MeshBuilder()) {
        auto vb = GetMeshVertexBuffer(mb);
        auto vd = GetVertexBufferDeclaration(vb);

        if(vd == vertexDecl) {
            invalidatedMeshes.insert(mb);
        }
    }
}

LocalFunction(OnMeshBuilderChildChanged, void, Entity child) {
    auto mesh = GetOwnership(child).Owner;
    invalidatedMeshes.insert(mesh);
}

LocalFunction(OnMeshBuilderChanged, void, Entity mb) {
    invalidatedMeshes.insert(mb);
}

BeginUnit(MeshBuilder)
    BeginComponent(MeshBuilderVertex)
        RegisterProperty(v3f, MeshBuilderVertexPosition)
        RegisterProperty(v3f, MeshBuilderVertexNormal)
        RegisterProperty(v2f, MeshBuilderVertexTexCoord0)
        RegisterProperty(v2f, MeshBuilderVertexTexCoord1)
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

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MeshBuilderVertexPosition()), OnMeshBuilderChildChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MeshBuilderVertexNormal()), OnMeshBuilderChildChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MeshBuilderVertexTexCoord0()), OnMeshBuilderChildChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MeshBuilderVertexTexCoord1()), OnMeshBuilderChildChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MeshBuilderVertexColor0()), OnMeshBuilderChildChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MeshBuilderIndexVertexIndex()), OnMeshBuilderChildChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_IndexBufferLong()), OnMeshBuilderChildChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MeshBuilderVertices()), OnMeshBuilderChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_MeshBuilderIndices()), OnMeshBuilderChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnMeshValidation, AppLoopOf_ResourcePreparation())
EndUnit()