//
// Created by Kim on 04-10-2018.
//

#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include "MeshBuilder.h"
#include "Mesh.h"
#include <Rendering/RenderContext.h>
#include <Rendering/RenderingModule.h>

static eastl::set<Entity> invalidatedMeshes;

static void BuildVertexBuffer(Entity mesh) {
    auto meshData = GetMesh(mesh);
    auto meshBuilderData = GetMeshBuilder(mesh);
    auto vertexBufferData = GetVertexBuffer(meshData.MeshVertexBuffer);
    auto vertexDeclarationData = GetVertexDeclaration(vertexBufferData.VertexBufferDeclaration);
    auto vb = meshData.MeshVertexBuffer;
    auto decl = vertexBufferData.VertexBufferDeclaration;

    auto vertexStride = 0;
    for(auto attrib : vertexDeclarationData.VertexDeclarationAttributes) {
        vertexStride += GetTypeSize(GetVertexAttribute(attrib).VertexAttributeType);
    }

    auto vertexBufferSize = vertexStride * meshBuilderData.MeshBuilderVertices.GetSize();
    auto vertexData = (char*)malloc(vertexBufferSize);

    auto attribOffset = 0;
    for(auto attrib : vertexDeclarationData.VertexDeclarationAttributes) {
        auto vertexOffset = 0;

        auto attribData = GetVertexAttribute(attrib);
        auto attribSize = GetTypeSize(attribData.VertexAttributeType);

        for(auto vertex : meshBuilderData.MeshBuilderVertices) {
            auto meshBuilderVertexData = GetMeshBuilderVertex(vertex);
            auto offset = vertexOffset + attribOffset;

            Variant value;
            switch(attribData.VertexAttributeUsage) {
                case VertexAttributeUsage_Position:
                    value.type = TypeOf_v3f;
                    value.as_v3f = meshBuilderVertexData.MeshBuilderVertexPosition;
                    break;
                case VertexAttributeUsage_Normal:
                    value.type = TypeOf_v3f;
                    value.as_v3f = meshBuilderVertexData.MeshBuilderVertexNormal;
                    break;
                case VertexAttributeUsage_TexCoord0:
                    value.type = TypeOf_v2f;
                    value.as_v2f = meshBuilderVertexData.MeshBuilderVertexTexCoord0;
                    break;
                case VertexAttributeUsage_TexCoord1:
                    value.type = TypeOf_v2f;
                    value.as_v2f = meshBuilderVertexData.MeshBuilderVertexTexCoord1;
                    break;
                case VertexAttributeUsage_Color0:
                    value.type = TypeOf_rgba8;
                    value.as_rgba8 = meshBuilderVertexData.MeshBuilderVertexColor0;
                    break;
            }

            value = Cast(value, attribData.VertexAttributeType);

            memcpy(vertexData + offset, &value.data, attribSize);

            vertexOffset += vertexStride;
        }

        attribOffset += GetTypeSize(attribData.VertexAttributeType);
    }

    auto streamData = GetStream(vb);
    if(!streamData.StreamPath) {
        streamData.StreamPath = StringFormatV("memory://%s.vtb", GetIdentification(vb).Uuid);
        SetStream(vb, streamData);
    }

    if(StreamOpen(vb, StreamMode_Write)) {
        StreamWrite(vb, vertexBufferSize, vertexData);
        StreamClose(vb);
    }

    free(vertexData);
}

static void BuildIndexBuffer(Entity mesh) {
    auto meshData = GetMesh(mesh);
    auto meshBuilderData = GetMeshBuilder(mesh);
    auto ib = meshData.MeshIndexBuffer;

    auto isIndexLong = GetIndexBuffer(ib).IndexBufferLong;

    auto indexBufferSize = (isIndexLong ? 4 : 2) * meshBuilderData.MeshBuilderIndices.GetSize();
    auto indexData = (char*)malloc(indexBufferSize);

    auto offset = 0;
    if(isIndexLong) {
        for(auto meshBuilderIndex : meshBuilderData.MeshBuilderIndices) {
            auto index = GetMeshBuilderIndex(meshBuilderIndex);

            *(u32*)(indexData + offset) = index.MeshBuilderIndexVertexIndex;
            offset += 4;
        }
    } else {
        for(auto meshBuilderIndex : meshBuilderData.MeshBuilderIndices) {
            auto index = GetMeshBuilderIndex(meshBuilderIndex);

            *(u16*)(indexData + offset) = (u16)index.MeshBuilderIndexVertexIndex;
            offset += 2;
        }
    }

    auto streamData = GetStream(ib);
    if(!streamData.StreamPath) {
        streamData.StreamPath = StringFormatV("memory://%s.idb", GetIdentification(ib).Uuid);
        SetStream(ib, streamData);
    }

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

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    for(auto& mesh : invalidatedMeshes) {
        BuildMesh(mesh);
    }

    invalidatedMeshes.clear();
}

static void OnVertexDeclarationChanged(Entity vertexDecl, const VertexDeclaration& oldData, const VertexDeclaration& newData) {
    for_entity(mb, ComponentOf_MeshBuilder()) {
        auto vb = GetMesh(mb).MeshVertexBuffer;
        auto vd = GetVertexBuffer(vb).VertexBufferDeclaration;

        if(vd == vertexDecl) {
            invalidatedMeshes.insert(mb);
        }
    }
}

static void OnMeshBuilderVertexChanged(Entity vertex, const MeshBuilderVertex& oldData, const MeshBuilderVertex& newData) {
    auto mesh = GetOwnership(vertex).Owner;
    invalidatedMeshes.insert(mesh);
}

static void OnMeshBuilderIndexChanged(Entity index, const MeshBuilderIndex& oldData, const MeshBuilderIndex& newData) {
    auto mesh = GetOwnership(index).Owner;
    invalidatedMeshes.insert(mesh);
}

static void OnMeshBuilderChanged(Entity mb, const MeshBuilder& oldData, const MeshBuilder& newData) {
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

    RegisterSystem(OnVertexDeclarationChanged, ComponentOf_VertexDeclaration())
    RegisterSystem(OnMeshBuilderVertexChanged, ComponentOf_MeshBuilderVertex())
    RegisterSystem(OnMeshBuilderIndexChanged, ComponentOf_MeshBuilderIndex())
    RegisterSystem(OnMeshBuilderChanged, ComponentOf_MeshBuilder())
    RegisterDeferredSystem(OnMeshBuilderChanged, ComponentOf_MeshBuilder(), AppLoopOrder_ResourcePreparation)
EndUnit()