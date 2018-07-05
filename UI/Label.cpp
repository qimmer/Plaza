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

DefineComponent(Label)
    Dependency(Widget)
    DefineProperty(Entity, LabelFont)
    DefineProperty(StringRef, LabelText)
    DefineProperty(v2f, LabelAlignment)
EndComponent()

DefineComponentPropertyReactive(Label, Entity, LabelFont)
DefineComponentPropertyReactive(Label, StringRef, LabelText)
DefineComponentPropertyReactive(Label, v2f, LabelAlignment)

static void RebuildLabel(Entity entity) {
    if(!HasLabel(entity)) return;

    auto data = GetLabel(entity);

    if(IsEntityValid(data->LabelFont)) {
        SetMeshInstanceMaterial(entity, GetFontMaterial(data->LabelFont));

        auto length = data->LabelText.length();
        auto vertices = (FontVertex*)malloc(sizeof(FontVertex) * data->LabelText.length() * 6);
        auto numVertices = GetFontGlyphData(data->LabelFont, data->LabelText.c_str(), {0.0f, 0.0f}, vertices, data->LabelText.length() * 6);

        v2f min = {FLT_MAX, FLT_MAX}, max = {FLT_MIN, FLT_MIN};
        for(auto i = 0; i < numVertices; ++i) {
            min.x = std::min(min.x, vertices[i].Position.x);
            min.y = std::min(min.y, vertices[i].Position.y);

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

static void OnLabelAdded(Entity entity) {
    auto data = GetLabel(entity);
    data->Mesh = CreateMesh(entity, "Mesh");
    data->VertexBuffer = CreateVertexBuffer(data->Mesh, "VertexBuffer");
    SetVertexBufferDeclaration(data->VertexBuffer, GetFontVertexDeclaration());
    SetMeshVertexBuffer(data->Mesh, data->VertexBuffer);

    SetMeshInstanceMesh(entity, data->Mesh);
}

DefineService(Label)
    Subscribe(FontChanged, RebuildLabelsWithFont)
    Subscribe(WidgetSizeChanged, RebuildLabel)
    Subscribe(LabelChanged, RebuildLabel)
    Subscribe(LabelAdded, OnLabelAdded)
EndService()