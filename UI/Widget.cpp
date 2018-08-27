//
// Created by Kim Johannsen on 03-04-2018.
//

#include <Scene/MeshInstance.h>
#include <Scene2D/Transform2D.h>
#include <Rendering/VertexDeclaration.h>
#include <Rendering/VertexAttribute.h>
#include <Rendering/Mesh.h>
#include <Rendering/VertexBuffer.h>
#include <Foundation/Stream.h>
#include <Rendering/Shader.h>
#include <Rendering/UniformState.h>
#include <Rendering/Uniform.h>
#include <Rendering/Material.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include "Widget.h"

RegisterEvent(WidgetMouseDown)
RegisterEvent(WidgetMouseUp)

BeginEnum(WidgetChildLayout, false)
    RegisterFlag(WidgetChildLayout_Horizontal)
    RegisterFlag(WidgetChildLayout_Vertical)
EndEnum()

BeginEnum(WidgetSizing, false)
    RegisterFlag(WidgetSizing_Fixed)
    RegisterFlag(WidgetSizing_Weighted)
EndEnum()

Entity Widget9x9Mesh, WidgetMesh,
        WidgetVertexDeclaration,
        WidgetVertexShader,
        WidgetPixelShader,
        WidgetShaderDeclaration,
        WidgetTextureUniform,
        WidgetSizeUniform,
        WidgetBorderSizeUniform,
        WidgetMaterial,
        WhiteTexture;

struct Widget {
    Widget() : WidgetWeight({1.0f, 1.0f}) {}

    v2i WidgetSize, WidgetMinimumSize;
    v2f WidgetWeight;
    u8 WidgetChildLayout, WidgetSizing;
    Entity WidgetTexture, WidgetTextureState, WidgetSizeState, WidgetBorderSizeState;
};

BeginUnit(Widget)
    BeginComponent(Widget)
    RegisterBase(MeshInstance)
    RegisterProperty(v2i, WidgetMinimumSize))
    RegisterProperty(v2i, WidgetSize))
    RegisterProperty(v2f, WidgetWeight))
    RegisterProperty(Entity, WidgetTexture))
    RegisterPropertyEnum(u8, WidgetChildLayout, WidgetChildLayout)
    RegisterPropertyEnum(u8, WidgetSizing, WidgetSizing)
EndComponent()
EndUnit()
(v2i, WidgetSize)
RegisterProperty(v2i, WidgetMinimumSize)
RegisterProperty(v2f, WidgetWeight)
RegisterProperty(u8, WidgetChildLayout)
RegisterProperty(u8, WidgetSizing)
RegisterProperty(Entity, WidgetTexture)
DefineComponentChild(Widget, UniformState, WidgetTextureState)
DefineComponentChild(Widget, UniformState, WidgetSizeState)
DefineComponentChild(Widget, UniformState, WidgetBorderSizeState)

static void UpdateChildrenLayout(Entity parentWidget) {
    auto parentData = GetWidget(parentWidget);

    v2f weightSum = {0.0f, 0.0f};
    for_children(childWidget, parentWidget) {
        if(!HasComponent(childWidget, ComponentOf_Widget())) continue;

        auto childData = GetWidget(childWidget);
        weightSum.x += childData->WidgetWeight.x;
        weightSum.y += childData->WidgetWeight.y;
    }

    v2i position = {0, 0};
    for_children(childWidget, parentWidget) {
        if(!HasComponent(childWidget, ComponentOf_Widget())) continue;

        auto childData = GetWidget(childWidget);

        SetPosition2D(childWidget, {
            (float)position.x,
            (float)position.y
        });

        v2i newSize;
        if(parentData->WidgetSizing == WidgetSizing_Weighted) {
            newSize.x = s32(parentData->WidgetSize.x * childData->WidgetWeight.x);
            newSize.y = s32(parentData->WidgetSize.y * childData->WidgetWeight.y);
        } else {
            newSize = parentData->WidgetMinimumSize;
        }

        if(parentData->WidgetChildLayout == WidgetChildLayout_Horizontal) {
            position.x += childData->WidgetSize.x;
            newSize.y = parentData->WidgetSize.y;
        } else {
            position.y += childData->WidgetSize.y;
            newSize.x = parentData->WidgetSize.x;
        }

        SetWidgetSize(childWidget, newSize);
    }
}

LocalFunction(OnWidgetSizeChanged, void, Entity widget, v2i oldSize, v2i newSize) {
    UpdateChildrenLayout(widget);
}

LocalFunction(OnWidgetAdded, void, Entity component, Entity widget) {
    SetMeshInstanceMaterial(widget, WidgetMaterial);
    SetMeshInstanceMesh(widget, WidgetMesh);

    auto textureState = GetWidgetTextureState(widget);
    SetUniformStateUniform(textureState, WidgetTextureUniform);
    SetUniformStateTexture(textureState, WhiteTexture);

    auto sizeState = GetWidgetSizeState(widget);
    SetUniformStateUniform(sizeState, WidgetSizeUniform);
}

static void CreateWidgetMesh(Entity mesh) {
    AddMesh(mesh);

    v2f vertices[] = {
        {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f},
        {1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f},
        {1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f},

        {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f},
        {0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f},
        {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}
    };

    auto vb = CreateVertexBuffer(mesh, "VertexBuffer");
    SetVertexBufferDeclaration(vb, WidgetVertexDeclaration);
    StreamOpen(vb, StreamMode_Write);
    StreamWrite(vb, sizeof(v2f) * 6, vertices);
    StreamClose(vb);

    SetMeshVertexBuffer(mesh, vb);
    SetMeshNumVertices(mesh, 6);
}

static void CreateWidgetVertexDeclaration(Entity vd) {
    AddVertexDeclaration(vd);

    auto pos = CreateVertexAttribute(vd, "Position");
    SetVertexAttributeType(pos, TypeOf_v2f());
    SetVertexAttributeUsage(pos, VertexAttributeUsage_Position);

    auto texCoord = CreateVertexAttribute(vd, "TexCoord");
    SetVertexAttributeType(texCoord, TypeOf_v2f());
    SetVertexAttributeUsage(texCoord, VertexAttributeUsage_TexCoord0);

    auto weight = CreateVertexAttribute(vd, "SizeWeight");
    SetVertexAttributeType(weight, TypeOf_v2f());
    SetVertexAttributeUsage(weight, VertexAttributeUsage_TexCoord1);
}

static void CreateVertexShader(Entity vs) {
    SetStreamPath(vs, "res://Assets/Shaders/widget.vs");
    SetShaderType(vs, ShaderType_Vertex);
    SetShaderDeclaration(vs, WidgetShaderDeclaration);
}

static void CreatePixelShader(Entity ps) {
    SetStreamPath(ps, "res://Assets/Shaders/widget.ps");
    SetShaderType(ps, ShaderType_Pixel);
    SetShaderDeclaration(ps, WidgetShaderDeclaration);
}

static void CreateShaderDeclaration(Entity sd) {
    SetStreamPath(sd, "res://Assets/Shaders/widget.var");
}

static void CreateWidgetTextureUniform(Entity uf) {
    SetUniformName(uf, "s_tex");
    SetUniformType(uf, TypeOf_Entity());
}

static void CreateWidgetSizeUniform(Entity uf) {
    SetUniformName(uf, "u_widgetSize");
    SetUniformType(uf, TypeOf_Entity());
}

static void CreateWidgetBorderSizeUniform(Entity uf) {
    SetUniformName(uf, "u_borderSize");
    SetUniformType(uf, TypeOf_Entity());
}

static void CreateWhiteTexture(Entity texture) {
    SetTextureFormat(texture, TextureFormat_RGBA8);
    SetTextureSize2D(texture, {1, 1});

    auto white = (u32)0xFFFFFFFF;
    StreamOpen(texture, StreamMode_Write);
    StreamWrite(texture, sizeof(u32), &white);
    StreamClose(texture);
}

static void CreateWidgetMaterial(Entity material) {
    SetMaterialVertexShader(material, WidgetVertexShader);
    SetMaterialPixelShader(material, WidgetPixelShader);
    SetMaterialBlendMode(material, RenderState_STATE_BLEND_ALPHA);
}

DefineService(Widget)
    RegisterSubscription(WidgetSizeChanged, OnWidgetSizeChanged, 0)
    RegisterSubscription(WidgetAdded, OnWidgetAdded, 0)

    ServiceEntity(WidgetVertexDeclaration, CreateWidgetVertexDeclaration)
    ServiceEntity(WidgetMesh, CreateWidgetMesh)
    ServiceEntity(WidgetVertexShader, CreateVertexShader)
    ServiceEntity(WidgetPixelShader, CreatePixelShader)
    ServiceEntity(WidgetShaderDeclaration, CreateShaderDeclaration)
    ServiceEntity(WidgetTextureUniform, CreateWidgetTextureUniform)
    ServiceEntity(WidgetSizeUniform, CreateWidgetSizeUniform)
    ServiceEntity(WidgetBorderSizeUniform, CreateWidgetBorderSizeUniform)
    ServiceEntity(WidgetMaterial, CreateWidgetMaterial)
    ServiceEntity(WhiteTexture, CreateWhiteTexture)
EndService()
