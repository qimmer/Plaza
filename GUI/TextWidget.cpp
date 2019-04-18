//
// Created by Kim on 08-01-2019.
//

#include <Foundation/Stream.h>
#include <Foundation/AppLoop.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/Material.h>
#include <Rendering/Uniform.h>
#include <Rendering/Mesh.h>
#include <Rendering/Renderable.h>
#include <Rendering/RenderContext.h>
#include <Core/Debug.h>
#include <Core/Algorithms.h>
#include <Core/Identification.h>
#include "Font.h"
#include "Widget.h"
#include "TextWidget.h"
#include <Json/NativeUtils.h>

static void RebuildTextWidget(Entity entity) {
    eastl::fixed_vector<v4f, 512> colors;

    if (!HasComponent(entity, ComponentOf_TextWidget())) return;

	static auto vertexDecl = FindEntityByUuid("Gui.Font.VertexDeclaration");

    auto data = GetTextWidget(entity);

	auto font = data.TextWidgetFont;
    auto text = data.TextWidgetText;

    if (IsEntityValid(font)) {
        auto length = text ? strlen(text) : 0;

        colors.resize(length, *(v4f*)&data.TextWidgetColor);
        
        auto vertices = (FontVertex *) alloca(sizeof(FontVertex) * length * 6);
        v2f size;
        auto numVertices = GetFontGlyphData(font, text, colors.data(), {0.0f, 0.0f}, &size, vertices,
                                            length * 6);

        v2f min = {FLT_MAX, FLT_MAX}, max = {FLT_MIN, FLT_MIN};
        for (auto i = 0; i < numVertices; ++i) {
            min.x = Min(min.x, vertices[i].Position.x);
            min.y = Min(min.y, vertices[i].Position.y);

            max.x = Max(max.x, vertices[i].Position.x);
            max.y = Max(max.y, vertices[i].Position.y);
        }

        // Set size to text extent
        SetRect2D(entity, {{Max(s32(size.x), 0), Max(s32(size.y), 0)}});

        auto textWidgetMesh = data.TextWidgetMesh;
        auto meshData = GetMesh(textWidgetMesh);
        auto vertexBuffer = meshData.MeshVertexBuffer;
        auto vertexBufferData = GetVertexBuffer(vertexBuffer);
        vertexBufferData.VertexBufferDeclaration = vertexDecl;
        SetVertexBuffer(vertexBuffer, vertexBufferData);

        auto streamData = GetStream(vertexBuffer);
        if(!streamData.StreamPath || !strlen(streamData.StreamPath)) {
            streamData.StreamPath = StringFormatV("memory://%s.vtb", GetIdentification(vertexBuffer).Uuid);
            SetStream(vertexBuffer, streamData);
        }

        if(!StreamOpen(vertexBuffer, StreamMode_Write)) {
            return;
        }

        StreamWrite(vertexBuffer, numVertices * sizeof(FontVertex), vertices);
        StreamClose(vertexBuffer);

        meshData.MeshSubMeshes.SetSize(1);
        if(!meshData.MeshSubMeshes[0]) {
            meshData.MeshSubMeshes[0] = CreateEntity();
        }

        auto subMeshData = GetSubMesh(meshData.MeshSubMeshes[0]);
        subMeshData.SubMeshNumVertices = numVertices;
        subMeshData.SubMeshPrimitiveType = PrimitiveType_TRIANGLELIST;
        SetSubMesh(meshData.MeshSubMeshes[0], subMeshData);

        auto renderableData = GetRenderable(entity);
        renderableData.RenderableSubMesh = meshData.MeshSubMeshes[0];
        SetRenderable(entity, renderableData);
    }
}

static void OnTextWidgetChanged(Entity textWidget, const TextWidget& oldData, const TextWidget& newData) {
    RebuildTextWidget(textWidget);
}

BeginUnit(TextWidget)
    BeginComponent(TextWidget)
        RegisterBase(Widget)
        RegisterProperty(StringRef, TextWidgetText)
        RegisterReferenceProperty(Font, TextWidgetFont)
        RegisterProperty(rgba32, TextWidgetColor)
        BeginChildProperty(TextWidgetMesh)

        EndChildProperty()
    EndComponent()

    BeginPrefab(TextWidget)
        Renderable renderableData;
        renderableData.RenderableMaterial = FindEntityByUuid("Gui.Font.Material");
        SetRenderable(prefab, renderableData);
    EndPrefab()

    RegisterSystem(OnTextWidgetChanged, ComponentOf_TextWidget())
EndUnit()
