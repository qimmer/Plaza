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

struct Label {
    Entity LabelFont;
    String LabelText;
    Entity Mesh, VertexBuffer;
};

DefineComponent(Label)
    Dependency(MeshInstance)
    DefineProperty(Entity, LabelFont)
    DefineProperty(StringRef, LabelText)
EndComponent()

DefineComponentPropertyReactive(Label, Entity, LabelFont)
DefineComponentPropertyReactive(Label, StringRef, LabelText)

static void RebuildLabel(Entity entity) {
    auto data = GetLabel(entity);

    if(IsEntityValid(data->LabelFont)) {
        SetMeshInstanceMaterial(entity, GetFontMaterial(data->LabelFont));

        auto length = data->LabelText.length();
        auto vertices = (FontVertex*)malloc(sizeof(FontVertex) * data->LabelText.length() * 6);
        auto numVertices = GetFontGlyphData(data->LabelFont, data->LabelText.c_str(), {0.0f, 0.0f}, vertices, data->LabelText.length() * 6);

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
    Subscribe(LabelChanged, RebuildLabel)
    Subscribe(LabelAdded, OnLabelAdded)
EndService()