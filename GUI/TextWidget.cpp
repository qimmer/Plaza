//
// Created by Kim on 08-01-2019.
//

#include <Foundation/Stream.h>
#include <Rendering/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/Material.h>
#include <Rendering/Uniform.h>
#include <Rendering/Mesh.h>
#include <Rendering/Renderable.h>
#include <Core/Debug.h>
#include <Foundation/Invalidation.h>
#include <Core/Algorithms.h>
#include <Core/Identification.h>
#include "Font.h"
#include "Widget.h"
#include "TextWidget.h"
#include <Json/NativeUtils.h>

static void RebuildTextWidget(Entity entity) {
    Vector<v4f> colors;

    if (!HasComponent(entity, ComponentOf_TextWidget())) return;

    auto data = GetTextWidgetData(entity);

    auto font = GetTextWidgetFont(entity);
    auto text = GetTextWidgetText(entity);

    if (IsEntityValid(font)) {
        if(!text) {
            text = Intern("");
        }

        auto length = strlen(text);

        colors.resize(length);
        for(auto i = 0; i < length; ++i) {
            colors[i] = *(v4f*)&data->TextWidgetColor;
        }
        auto vertices = (FontVertex *) malloc(sizeof(FontVertex) * length * 6);
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

        auto textWidgetMesh = GetTextWidgetMesh(entity);
        auto vertexBuffer = GetMeshVertexBuffer(textWidgetMesh);
        SetVertexBufferDeclaration(vertexBuffer, FindEntityByUuid("Gui.Font.VertexDeclaration"));

        auto currentPath = GetStreamPath(vertexBuffer);
        if(!currentPath || !strlen(currentPath)) {
            char path[1024];
            snprintf(path, sizeof(path), "memory://%s.vtb", GetUuid(vertexBuffer));
            SetStreamPath(vertexBuffer, path);
        }

        if(!StreamOpen(vertexBuffer, StreamMode_Write)) {
            free(vertices);
            return;
        }

        StreamWrite(vertexBuffer, numVertices * sizeof(FontVertex), vertices);
        StreamClose(vertexBuffer);

        SetNumMeshSubMeshes(textWidgetMesh, 1);
        auto subMesh = *GetMeshSubMeshes(textWidgetMesh, NULL);

        SetSubMeshNumVertices(subMesh, numVertices);
        SetSubMeshPrimitiveType(subMesh, PrimitiveType_TRIANGLELIST);
        free(vertices);

        SetRenderableSubMesh(entity, subMesh);
    }
}

LocalFunction(OnValidateMeshes, void, Entity component) {
    for_entity(textWidget, data, TextWidget) {
        if(IsDirty(textWidget)) {
            RebuildTextWidget(textWidget);
        }
    }
}

LocalFunction(OnValidateTextures, void, Entity component) {
    for_entity(font, data, Font) {
        if(IsDirty(font)) {
            for_entity(textWidget, data2, TextWidget) {
                    if(GetTextWidgetFont(textWidget) == font) {
                        RebuildTextWidget(textWidget);
                    }
            }
        }
    }
}

LocalFunction(OnTextWidgetChanged, void, Entity textWidget) {
    RebuildTextWidget(textWidget);
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
    RegisterSubscription(EventOf_Validate(), OnValidateMeshes, ComponentOf_Mesh())
    RegisterSubscription(EventOf_Validate(), OnValidateTextures, ComponentOf_Texture())
EndUnit()
