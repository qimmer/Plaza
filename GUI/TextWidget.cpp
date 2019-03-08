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

static eastl::set<Entity> invalidatedTextWidgets;

static void RebuildTextWidget(Entity entity) {
    Vector<v4f, 512> colors;

    if (!HasComponent(entity, ComponentOf_TextWidget())) return;

	static auto vertexDecl = FindEntityByUuid("Gui.Font.VertexDeclaration");

    auto data = GetTextWidgetData(entity);

	auto font = data->TextWidgetFont;
    auto text = data->TextWidgetText;

    if (IsEntityValid(font)) {
        auto length = text ? strlen(text) : 0;

        colors.resize(length, *(v4f*)&data->TextWidgetColor);
        
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
        SetSize2D(entity, {Max(s32(size.x), 0), Max(s32(size.y), 0)});

        auto textWidgetMesh = data->TextWidgetMesh;
        auto vertexBuffer = GetMeshVertexBuffer(textWidgetMesh);
        SetVertexBufferDeclaration(vertexBuffer, vertexDecl);

        auto currentPath = GetStreamPath(vertexBuffer);
        if(!currentPath || !strlen(currentPath)) {
            char path[1024];
            snprintf(path, sizeof(path), "memory://%s.vtb", GetUuid(vertexBuffer));
            SetStreamPath(vertexBuffer, path);
        }

        if(!StreamOpen(vertexBuffer, StreamMode_Write)) {
            return;
        }

        StreamWrite(vertexBuffer, numVertices * sizeof(FontVertex), vertices);
        StreamClose(vertexBuffer);

        SetNumMeshSubMeshes(textWidgetMesh, 1);
        auto subMesh = GetMeshSubMeshes(textWidgetMesh)[0];

        SetSubMeshNumVertices(subMesh, numVertices);
        SetSubMeshPrimitiveType(subMesh, PrimitiveType_TRIANGLELIST);
        SetRenderableSubMesh(entity, subMesh);
    }
}

LocalFunction(OnValidateMeshes, void) {
    for(auto& textWidget : invalidatedTextWidgets) {
        RebuildTextWidget(textWidget);
    }

    invalidatedTextWidgets.clear();
}

LocalFunction(OnTextWidgetChanged, void, Entity textWidget) {
    invalidatedTextWidgets.insert(textWidget);
}

BeginUnit(TextWidget)
    BeginComponent(TextWidget)
        RegisterBase(Widget)
        RegisterProperty(StringRef, TextWidgetText)
        RegisterReferenceProperty(Font, TextWidgetFont)
        RegisterProperty(rgba32, TextWidgetColor)
        RegisterChildProperty(Mesh, TextWidgetMesh)

        ComponentTemplate({
            "RenderableMaterial": "Gui.Font.Material"
        })
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextWidgetText()), OnTextWidgetChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextWidgetFont()), OnTextWidgetChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TextWidgetColor()), OnTextWidgetChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnValidateMeshes, AppLoopOf_ResourcePreparation())
EndUnit()
