//
// Created by Kim Johannsen on 04-04-2018.
//

#include <Scene/MeshInstance.h>
#include <Rendering/Mesh.h>
#include <Core/String.h>
#include <Rendering/VertexBuffer.h>
#include <Foundation/Stream.h>
#include <Scene/MeshInstance.h>
#include "Label.h"
#include "Font.h"
#include "Widget.h"
#include <cglm/cglm.h>
#include <cglm/vec3.h>

struct Label {
    Entity LabelFont;
    String LabelText;
    v2f LabelAlignment;
    Entity Mesh, VertexBuffer;
};

BeginUnit(Label)
    BeginComponent(Label)
    RegisterBase(Widget)
    RegisterProperty(Entity, LabelFont))
    RegisterProperty(StringRef, LabelText))
    RegisterProperty(v2f, LabelAlignment))
EndComponent()
EndUnit()
(Entity, LabelFont)
RegisterProperty(StringRef, LabelText)
RegisterProperty(v2f, LabelAlignment)

static void RebuildLabel(Entity entity) {
    if(!HasComponent(entity, ComponentOf_Label())) return;

    auto data = GetLabelData(entity);

    if(IsEntityValid(data->LabelFont)) {
        SetMeshInstanceMaterial(entity, GetFontMaterial(data->LabelFont));

        auto length = data->LabelText.length();
        auto vertices = (FontVertex*)malloc(sizeof(FontVertex) * data->LabelText.length() * 6);
        auto numVertices = GetFontGlyphData(data->LabelFont, data->LabelText.c_str(), {0.0f, 0.0f}, vertices, data->LabelText.length() * 6);

        v2f min = {FLT_MAX, FLT_MAX}, max = {FLT_MIN, FLT_MIN};
        for(auto i = 0; i < numVertices; ++i) {
            min.x = Min(min.x, vertices[i].Position.x);
            min.y = Min(min.y, vertices[i].Position.y);

            max.x = std::max(max.x, vertices[i].Position.x);
            max.y = std::max(max.y, vertices[i].Position.y);
        }

        Assert(StreamOpen(data->VertexBuffer, StreamMode_Write));
        StreamWrite(data->VertexBuffer, numVertices * sizeof(FontVertex), vertices);
        StreamClose(data->VertexBuffer);

        SetMeshNumVertices(data->Mesh, numVertices);
        free(vertices);
    }
}

static void RebuildLabelsWithFont(Entity font) {
    for_entity_data(label, data, Label) {
        if(data->LabelFont == font) {
            RebuildLabel(label);
        }
    }
}

LocalFunction(OnLabelAdded, void, Entity component, Entity entity) {
    auto data = GetLabelData(entity);
    data->Mesh = CreateMesh(entity, "Mesh");
    data->VertexBuffer = CreateVertexBuffer(data->Mesh, "VertexBuffer");
    SetVertexBufferDeclaration(data->VertexBuffer, GetFontVertexDeclaration());
    SetMeshVertexBuffer(data->Mesh, data->VertexBuffer);

    SetMeshInstanceMesh(entity, data->Mesh);
}

DefineService(Label)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Font()), RebuildLabelsWithFont, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetSize()), RebuildLabel, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Label()), RebuildLabel, 0)
    RegisterSubscription(LabelAdded, OnLabelAdded, 0)
EndService()