//
// Created by Kim Johannsen on 03-04-2018.
//

#include <Foundation/AppNode.h>
#include <Scene/Transform.h>
#include <Rendering/Mesh.h>
#include <Rendering/MeshBuilder.h>
#include <Rendering/Renderable.h>
#include <Rendering/RenderContext.h>
#include <Foundation/Stream.h>
#include <Rendering/Uniform.h>
#include <Rendering/Material.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include <Core/Identification.h>
#include <Foundation/Visibility.h>
#include <Animation/Transition.h>
#include <Json/NativeUtils.h>
#include <Scene/Scene.h>

#include "Widget.h"
#include "Font.h"

#include <cglm/cglm.h>

static eastl::set<Entity> invalidatedWidgetMeshes;

static void InvalidateWidgetMesh(Entity entity) {
    invalidatedWidgetMeshes.insert(entity);
}

static void UpdateWidgetBounds(Entity widget, Renderable& renderableData) {
    auto widgetSize = GetRect2D(widget).Size2D;
	auto transformData = GetWorldTransform(widget);
    
    v4f localMin = { 0.0f, 0.0f, 0.0f, 1.0f };
    v4f localMax = { (float)widgetSize.x, (float)widgetSize.y, 0.0f, 1.0f };

    glm_mat4_mulv((vec4*)&transformData.WorldTransformMatrix[0].x, &localMin.x, &renderableData.RenderableAABBMin.x);
    glm_mat4_mulv((vec4*)&transformData.WorldTransformMatrix[0].x, &localMax.x, &renderableData.RenderableAABBMax.x);
}

static void RebuildWidgetMesh(Entity mesh) {
    auto widgetMeshData = GetWidgetMesh(mesh);
    auto textureSize = GetSubTexture2D(widgetMeshData.WidgetMeshEnabledTexture).SubTexture2DSize;

    v2i borderSize = {widgetMeshData.WidgetMeshFixedBorderWidth, widgetMeshData.WidgetMeshFixedBorderWidth};
    v2i oneMinusBorderWidth = {textureSize.x - widgetMeshData.WidgetMeshFixedBorderWidth, textureSize.y - widgetMeshData.WidgetMeshFixedBorderWidth};

    WidgetVertex vertices[4*4];
    u16 indices[54];

    for(auto y = 0; y < 3; ++y) {
        for(auto x = 0; x < 3; ++x) {
            auto offset = (y * 3 + x) * 6;
            auto vertexIndex = (y * 4) + x;

            indices[offset + 0] = vertexIndex + 0 + 0;
            indices[offset + 1] = vertexIndex + 1 + 0;
            indices[offset + 2] = vertexIndex + 1 + 4;

            indices[offset + 3] = vertexIndex + 1 + 4;
            indices[offset + 4] = vertexIndex + 0 + 4;
            indices[offset + 5] = vertexIndex + 0 + 0;
        }
    }

    for(auto y = 0; y <= 1; ++y) {
        for(auto x = 0; x <= 1; ++x) {
            auto topLeftX =     x * 2    , topLeftY = y * 2;
            auto topRightX =    x * 2 + 1, topRightY = y * 2;
            auto bottomLeftX =  x * 2    , bottomLeftY = y * 2 + 1;
            auto bottomRightX = x * 2 + 1, bottomRightY = y * 2 + 1;

            auto& topLeft = vertices[topLeftY * 4 + topLeftX];
            auto& topRight = vertices[topRightY * 4 + topRightX];
            auto& bottomLeft = vertices[bottomLeftY * 4 + bottomLeftX];
            auto& bottomRight = vertices[bottomRightY * 4 + bottomRightX];

            topLeft.Position = {(float)(-x*borderSize.x), (float)(-y*borderSize.y), 0};
            topRight.Position = {(float)(-x*borderSize.x + borderSize.x), (float)(-y*borderSize.y), 0};
            bottomLeft.Position = {(float)(-x*borderSize.x), (float)(-y*borderSize.y + borderSize.y), 0};
            bottomRight.Position = {(float)(-x*borderSize.x + borderSize.x), (float)(-y*borderSize.y + borderSize.y), 0};

            topLeft.Alignment = topRight.Alignment = bottomLeft.Alignment = bottomRight.Alignment = {(float)x, (float)y};
            topLeft.Normal = topRight.Normal = bottomLeft.Normal = bottomRight.Normal = {0, 0, -1};

            topLeft.TexCoord = {(float)(x * oneMinusBorderWidth.x), (float)(y * oneMinusBorderWidth.y)};
            topRight.TexCoord = {(float)(x * oneMinusBorderWidth.x + widgetMeshData.WidgetMeshFixedBorderWidth), (float)(y * oneMinusBorderWidth.y)};
            bottomLeft.TexCoord = {(float)(x * oneMinusBorderWidth.x), (float)(y * oneMinusBorderWidth.y + widgetMeshData.WidgetMeshFixedBorderWidth)};
            bottomRight.TexCoord = {(float)(x * oneMinusBorderWidth.x + widgetMeshData.WidgetMeshFixedBorderWidth), (float)(y * oneMinusBorderWidth.y + widgetMeshData.WidgetMeshFixedBorderWidth)};
        }
    }

    for(auto i = 0; i < 16; ++i) {
        vertices[i].TexCoord.x /= textureSize.x;
        vertices[i].TexCoord.y /= textureSize.y;
    }

    auto meshData = GetMesh(mesh);

    auto vertexBufferData = GetVertexBuffer(meshData.MeshVertexBuffer);
    vertexBufferData.VertexBufferDeclaration = FindEntityByUuid("Gui.VertexDeclaration");
    SetVertexBuffer(meshData.MeshVertexBuffer, vertexBufferData);

    auto streamData = GetStream(meshData.MeshVertexBuffer);
    if(!streamData.StreamPath) {
        streamData.StreamPath = StringFormatV("memory://%s", GetIdentification(meshData.MeshVertexBuffer).Uuid);
        SetStream(meshData.MeshVertexBuffer, streamData);
    }

    if(StreamOpen(meshData.MeshVertexBuffer, StreamMode_Write)) {
        StreamWrite(meshData.MeshVertexBuffer, sizeof(vertices), vertices);
        StreamClose(meshData.MeshVertexBuffer);
    }

    streamData = GetStream(meshData.MeshIndexBuffer);
    if(!streamData.StreamPath) {
        streamData.StreamPath = StringFormatV("memory://%s", GetIdentification(meshData.MeshIndexBuffer).Uuid);
        SetStream(meshData.MeshIndexBuffer, streamData);
    }

    if(StreamOpen(meshData.MeshIndexBuffer, StreamMode_Write)) {
        StreamWrite(meshData.MeshIndexBuffer, sizeof(indices), indices);
        StreamClose(meshData.MeshIndexBuffer);
    }

    meshData.MeshSubMeshes.SetSize(1);
    if(!meshData.MeshSubMeshes[0]) {
        meshData.MeshSubMeshes[0] = CreateEntity();
    }

    SetSubMesh(meshData.MeshSubMeshes[0], {0, 0, 4*4, sizeof(indices) / sizeof(u16), PrimitiveType_TRIANGLELIST, RenderState_STATE_CULL_NONE});
    SetMesh(mesh, meshData);
}

static void UpdateBounds() {
    Widget data;
    for_entity_data(widget, ComponentOf_Widget(), &data) {
        auto renderableData = GetRenderable(widget);

        UpdateWidgetBounds(widget, renderableData);

        if(data.WidgetDepthOrder != 0.0f) {
            renderableData.RenderableScissor = {0, 0, 0, 0};
            SetRenderable(widget, renderableData);

            continue;
        }

        auto owner = GetOwnership(widget).Owner;
        renderableData.RenderableScissor = {INT_MIN, INT_MIN, INT_MAX, INT_MAX};
        while(HasComponent(owner, ComponentOf_Rect2D())) {
            auto ownerRect = GetRect2D(owner);
            auto transform = GetWorldTransform(owner);
            renderableData.RenderableScissor.x = Max(renderableData.RenderableScissor.x, transform.WorldTransformMatrix[3].x);
            renderableData.RenderableScissor.y = Max(renderableData.RenderableScissor.y, transform.WorldTransformMatrix[3].y);
            renderableData.RenderableScissor.z = Min(renderableData.RenderableScissor.z, transform.WorldTransformMatrix[3].x + ownerRect.Size2D.x);
            renderableData.RenderableScissor.w = Min(renderableData.RenderableScissor.w, transform.WorldTransformMatrix[3].y + ownerRect.Size2D.y);

            owner = GetOwnership(owner).Owner;
        }

        renderableData.RenderableScissor.z -= renderableData.RenderableScissor.x;
        renderableData.RenderableScissor.w -= renderableData.RenderableScissor.y;

        SetRenderable(widget, renderableData);
    }
}

static void OnWidgetStateChanged(Entity entity, const WidgetState& oldData, const WidgetState& newData) {
    auto widgetMesh = GetRenderable(entity).RenderableSubMesh;
    if (HasComponent(widgetMesh, ComponentOf_SubMesh())) {
        widgetMesh = GetOwnership(widgetMesh).Owner;
    }

    auto widgetMeshData = GetWidgetMesh(widgetMesh);

    auto interactableWidgetMeshData = GetInteractableWidgetMesh(widgetMesh);

    rgba32 color = widgetMeshData.WidgetMeshEnabledColor;
    glm_vec4_lerp(&color.r, &interactableWidgetMeshData.WidgetMeshFocusedColor.r, newData.WidgetInteractionState.y, &color.x);
    glm_vec4_lerp(&color.r, &interactableWidgetMeshData.WidgetMeshHoveredColor.r, newData.WidgetInteractionState.x, &color.x);
    glm_vec4_lerp(&color.r, &widgetMeshData.WidgetMeshSelectedColor.r, newData.WidgetState.y, &color.x);
    glm_vec4_lerp(&color.r, &interactableWidgetMeshData.WidgetMeshClickedColor.r, newData.WidgetInteractionState.z, &color.x);
    glm_vec4_lerp(&color.r, &widgetMeshData.WidgetMeshDisabledColor.r, newData.WidgetState.x, &color.x);

    auto state = newData;
    state.WidgetStateColor = color;
    SetWidgetState(entity, state);
}

static void OnWidgetMeshChanged(Entity entity, const WidgetMesh& oldData, const WidgetMesh& newData) {
    invalidatedWidgetMeshes.insert(entity);
}

static void OnWidgetChanged(Entity entity, const Widget& oldData, const Widget& newData) {
    auto stateData = GetWidgetState(entity);

    v3f newState = {
        newData.WidgetDisabled ? 1.0f : 0.0f,
        newData.WidgetSelected ? 1.0f : 0.0f,
        GetVisibility(entity).HierarchiallyHidden ? 1.0f : 0.0f
    };

    Transition(entity, PropertyOf_WidgetState(), MakeVariant(v2f, newState), stateData.WidgetStateTransitionDuration);
}

static void OnInteractableWidgetChanged(Entity entity, const InteractableWidget& oldData, const InteractableWidget& newData) {
    auto stateData = GetWidgetState(entity);

    v3f newState = {
            newData.WidgetHovered ? 1.0f : 0.0f,
        newData.WidgetFocused ? 1.0f : 0.0f,
        newData.WidgetClicked ? 1.0f : 0.0f,
    };

    Transition(entity, PropertyOf_WidgetInteractionState(), MakeVariant(v3f, newState), stateData.WidgetStateTransitionDuration);
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    for(auto& mesh : invalidatedWidgetMeshes) {
        RebuildWidgetMesh(mesh);
    }

    invalidatedWidgetMeshes.clear();

    UpdateBounds();
}

BeginUnit(Widget)
    BeginComponent(Rect2D)
        RegisterBase(Transform)
        RegisterProperty(v2i, Size2D)
    EndComponent()

    BeginComponent(Widget)
        RegisterBase(Rect2D)
        RegisterBase(Renderable)
        RegisterProperty(bool, WidgetDisabled)
        RegisterProperty(bool, WidgetSelected)
        RegisterProperty(float, WidgetDepthOrder)
    EndComponent()

    BeginComponent(InteractableWidget)
        RegisterBase(Widget)
        RegisterPropertyReadOnly(v2i, WidgetInteractionPoint)
        RegisterProperty(bool, WidgetHovered)
        RegisterProperty(bool, WidgetFocused)
        RegisterProperty(bool, WidgetClicked)
    EndComponent()

    BeginComponent(WidgetState)
        RegisterProperty(float, WidgetStateTransitionDuration)
        RegisterPropertyReadOnly(v3f, WidgetState)
        RegisterPropertyReadOnly(v3f, WidgetInteractionState)
        RegisterPropertyReadOnly(rgba32, WidgetStateColor)
    EndComponent()

    BeginComponent(WidgetMesh)
        RegisterBase(MeshBuilder)
        RegisterReferenceProperty(SubTexture2D, WidgetMeshEnabledTexture)
        RegisterReferenceProperty(SubTexture2D, WidgetMeshDisabledTexture)
        RegisterReferenceProperty(SubTexture2D, WidgetMeshSelectedTexture)
        RegisterProperty(rgba32, WidgetMeshEnabledColor)
        RegisterProperty(rgba32, WidgetMeshDisabledColor)
        RegisterProperty(rgba32, WidgetMeshSelectedColor)
        RegisterProperty(u16, WidgetMeshFixedBorderWidth)
    EndComponent()

    BeginComponent(InteractableWidgetMesh)
        RegisterBase(WidgetMesh)
        RegisterReferenceProperty(SubTexture2D, WidgetMeshHoveredTexture)
        RegisterReferenceProperty(SubTexture2D, WidgetMeshFocusedTexture)
        RegisterReferenceProperty(SubTexture2D, WidgetMeshClickedTexture)
        RegisterProperty(rgba32, WidgetMeshHoveredColor)
        RegisterProperty(rgba32, WidgetMeshFocusedColor)
        RegisterProperty(rgba32, WidgetMeshClickedColor)
    EndComponent()

	RegisterSystem(OnWidgetStateChanged, ComponentOf_WidgetState())
    RegisterSystem(OnWidgetMeshChanged, ComponentOf_WidgetMesh())
    RegisterSystem(OnWidgetChanged, ComponentOf_Widget())
    RegisterSystem(OnInteractableWidgetChanged, ComponentOf_InteractableWidget())
    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_BoundsUpdate)
EndUnit()
