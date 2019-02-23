//
// Created by Kim Johannsen on 03-04-2018.
//

#include <Foundation/AppNode.h>
#include <Scene/Transform.h>
#include <Rendering/Mesh.h>
#include <Rendering/MeshBuilder.h>
#include <Rendering/Renderable.h>
#include <Foundation/Stream.h>
#include <Rendering/Uniform.h>
#include <Rendering/Material.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include <Core/Identification.h>
#include <Foundation/Invalidation.h>
#include <Foundation/Visibility.h>
#include <Animation/Transition.h>
#include <Json/NativeUtils.h>
#include <Scene/Scene.h>

#include "Widget.h"
#include "Font.h"

#include <cglm/cglm.h>

static void UpdateWidgetBounds(Entity widget) {
    auto widgetSize = GetSize2D(widget);
    auto globalMat = GetTransformGlobalMatrix(widget);

    v4f localMin = { 0.0f, 0.0f, 0.0f, 1.0f };
    v4f localMax = { (float)widgetSize.x, (float)widgetSize.y, 0.0f, 1.0f };

    v4f globalMin, globalMax;

    glm_mat4_mulv((vec4*)&globalMat, &localMin.x, &globalMin.x);
    glm_mat4_mulv((vec4*)&globalMat, &localMax.x, &globalMax.x);

    SetRenderableAABBMin(widget, {globalMin.x, globalMin.y, globalMin.z});
    SetRenderableAABBMax(widget, {globalMax.x, globalMax.y, globalMax.z});
}

LocalFunction(OnSize2DChanged, void, Entity widget, v2i oldSize, v2i newSize) {
    if(!HasComponent(widget, ComponentOf_Widget())) return;

    UpdateWidgetBounds(widget);
}

LocalFunction(OnTransformGlobalChanged, void, Entity widget) {
    if(HasComponent(widget, ComponentOf_Widget())) {
        UpdateWidgetBounds(widget);
    }
}

LocalFunction(RebuildWidgetMesh, void, Entity mesh) {
    auto borderWidth = GetWidgetMeshFixedBorderWidth(mesh);
    auto texture = GetWidgetMeshEnabledTexture(mesh);
    auto textureSize = GetSubTexture2DSize(texture);

    v2i borderSize = {borderWidth, borderWidth};
    v2i oneMinusBorderWidth = {textureSize.x - borderWidth, textureSize.y - borderWidth};

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
            topRight.TexCoord = {(float)(x * oneMinusBorderWidth.x + borderWidth), (float)(y * oneMinusBorderWidth.y)};
            bottomLeft.TexCoord = {(float)(x * oneMinusBorderWidth.x), (float)(y * oneMinusBorderWidth.y + borderWidth)};
            bottomRight.TexCoord = {(float)(x * oneMinusBorderWidth.x + borderWidth), (float)(y * oneMinusBorderWidth.y + borderWidth)};
        }
    }

    for(auto i = 0; i < 16; ++i) {
        vertices[i].TexCoord.x /= textureSize.x;
        vertices[i].TexCoord.y /= textureSize.y;
    }

    auto vb = GetMeshVertexBuffer(mesh);
    auto ib = GetMeshIndexBuffer(mesh);
    auto vd = FindEntityByUuid("Gui.VertexDeclaration");

    SetVertexBufferDeclaration(vb, vd);

    auto currentPath = GetStreamPath(vb);
    if(!currentPath || !strlen(currentPath)) {
        char path[1024];
        snprintf(path, sizeof(path), "memory://%s", GetUuid(vb));
        SetStreamPath(vb, path);
    }

    if(StreamOpen(vb, StreamMode_Write)) {
        StreamWrite(vb, sizeof(vertices), vertices);
        StreamClose(vb);
    }

    currentPath = GetStreamPath(ib);
    if(!currentPath || !strlen(currentPath)) {
        char path[1024];
        snprintf(path, sizeof(path), "memory://%s", GetUuid(ib));
        SetStreamPath(ib, path);
    }

    if(StreamOpen(ib, StreamMode_Write)) {
        StreamWrite(ib, sizeof(indices), indices);
        StreamClose(ib);
    }

    SetNumMeshSubMeshes(mesh, 1);
    auto subMesh = GetMeshSubMeshes(mesh, NULL)[0];
    SetSubMeshPrimitiveType(subMesh, PrimitiveType_TRIANGLELIST);
    SetSubMeshNumIndices(subMesh, sizeof(indices) / sizeof(u16));
    SetSubMeshNumVertices(subMesh, 4*4);
}

LocalFunction(OnValidateMeshes, void, Entity component) {
    for_entity(mesh, meshData, WidgetMesh) {
        if(IsDirty(mesh)) {
            RebuildWidgetMesh(mesh);
        }
    }
}

LocalFunction(OnWidgetMeshAdded, void, Entity component, Entity entity) {
    SetNumMeshSubMeshes(entity, 1);
    Invalidate(entity);
}

LocalFunction(OnStateChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_Widget())) {
        auto data = GetWidgetData(entity);

        v3f newState = {
                data->WidgetDisabled ? 1.0f : 0.0f,
                data->WidgetSelected ? 1.0f : 0.0f,
                GetHidden(entity) ? 1.0f : 0.0f
        };

        Transition(entity, PropertyOf_WidgetState(), MakeVariant(v2f, newState), GetWidgetStateTransitionDuration(entity));
    }
}

LocalFunction(OnInteractableStateChanged, void, Entity entity) {
    auto data = GetInteractableWidgetData(entity);

    v3f newState = {
        data->WidgetHovered ? 1.0f : 0.0f,
        data->WidgetFocused ? 1.0f : 0.0f,
        data->WidgetClicked ? 1.0f : 0.0f,
    };

    Transition(entity, PropertyOf_WidgetInteractionState(), MakeVariant(v3f, newState), GetWidgetStateTransitionDuration(entity));
}

LocalFunction(OnStateUpdated, void, Entity entity) {
    if(!HasComponent(entity, ComponentOf_Widget())) return;

    auto state = GetWidgetState(entity);
    auto interactionState = GetWidgetInteractionState(entity);

    auto widgetMesh = GetRenderableSubMesh(entity);
    if (HasComponent(widgetMesh, ComponentOf_SubMesh())) {
        widgetMesh = GetOwner(widgetMesh);
    }

    auto widgetMeshData = GetWidgetMeshData(widgetMesh);
    if(!widgetMeshData) return;

    auto interactableWidgetMeshData = GetInteractableWidgetMeshData(widgetMesh);

    rgba32 color = widgetMeshData->WidgetMeshEnabledColor;

    if(interactableWidgetMeshData) {
        glm_vec4_lerp(&color.r, &interactableWidgetMeshData->WidgetMeshFocusedColor.r, interactionState.y, &color.x);
        glm_vec4_lerp(&color.r, &interactableWidgetMeshData->WidgetMeshHoveredColor.r, interactionState.x, &color.x);
        glm_vec4_lerp(&color.r, &widgetMeshData->WidgetMeshSelectedColor.r, state.y, &color.x);
        glm_vec4_lerp(&color.r, &interactableWidgetMeshData->WidgetMeshClickedColor.r, interactionState.z, &color.x);
    } else {
        glm_vec4_lerp(&color.r, &widgetMeshData->WidgetMeshSelectedColor.r, state.y, &color.x);
    }

    glm_vec4_lerp(&color.r, &widgetMeshData->WidgetMeshDisabledColor.r, state.x, &color.x);

    SetWidgetStateColor(entity, color);
}

BeginUnit(Widget)
    BeginComponent(Rect2D)
        RegisterBase(Transform)
        RegisterProperty(v2i, Size2D)
    EndComponent()

    BeginComponent(Widget)
        RegisterBase(Rect2D)
        RegisterBase(Renderable)
        RegisterProperty(Entity, WidgetModel)
        RegisterProperty(float, WidgetStateTransitionDuration)
        RegisterProperty(v3f, WidgetState)
        RegisterProperty(bool, WidgetDisabled)
        RegisterProperty(bool, WidgetSelected)
        RegisterPropertyReadOnly(rgba32, WidgetStateColor)

        ComponentTemplate({
            "RenderableMaterial": "Gui.Material"
        })
    EndComponent()

    BeginComponent(InteractableWidget)
        RegisterBase(Widget)
        RegisterProperty(v3f, WidgetInteractionState)
        RegisterProperty(v2i, WidgetInteractionPoint)
        RegisterProperty(bool, WidgetHovered)
        RegisterProperty(bool, WidgetFocused)
        RegisterProperty(bool, WidgetClicked)

        ComponentTemplate({
          "WidgetMeshHoveredColor": [0, 0, 0, 1],
          "WidgetMeshFocusedColor": [0, 0, 0, 1],
          "WidgetMeshClickedColor": [0, 0, 0, 1]
        })
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

        ComponentTemplate({
            "WidgetMeshEnabledColor": [0, 0, 0, 1],
            "WidgetMeshDisabledColor": [0, 0, 0, 1],
            "WidgetMeshSelectedColor": [0, 0, 0, 1]
        })
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

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Size2D()), OnSize2DChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TransformGlobalMatrix()), OnTransformGlobalChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshFixedBorderWidth()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshEnabledTexture()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshDisabledTexture()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshSelectedTexture()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshHoveredTexture()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshFocusedTexture()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshClickedTexture()), Invalidate, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetDisabled()), OnStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetSelected()), OnStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Hidden()), OnStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetHovered()), OnInteractableStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetFocused()), OnInteractableStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClicked()), OnInteractableStateChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetState()), OnStateUpdated, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetInteractionState()), OnStateUpdated, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderableSubMesh()), OnStateUpdated, 0)

    RegisterSubscription(EventOf_Validate(), OnValidateMeshes, ComponentOf_Mesh())
    RegisterSubscription(EventOf_EntityComponentAdded(), OnWidgetMeshAdded, ComponentOf_WidgetMesh())
EndUnit()
