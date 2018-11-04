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
#include "Widget.h"
#include "Font.h"

#include <cglm/cglm.h>

struct WidgetVertex {
    v3f Position, Normal;
    v2f TexCoord, Alignment;
};

static void UpdateChildrenLayout(Entity parentWidget) {
    auto parentData = GetWidgetData(parentWidget);

    if(parentData->WidgetChildLayout == WidgetChildLayout_Manual) return;

    u32 numChildWidgets = 0;
    auto childWidgets = GetChildren(parentWidget, &numChildWidgets);

    v2f weightSum = {0.0f, 0.0f};
    v2i fixedSum = {0, 0};
    for(auto i = 0; i < numChildWidgets; ++i) {
        auto childWidget = childWidgets[i];
        if(!HasComponent(childWidget, ComponentOf_Widget())) continue;

        auto childData = GetWidgetData(childWidget);

        if(childData->WidgetWeight.x != 0.0f) {
            weightSum.x += childData->WidgetWeight.x;
        } else {
            fixedSum.x += childData->WidgetMinimumSize.x;
        }

        if(childData->WidgetWeight.y != 0.0f) {
            weightSum.y += childData->WidgetWeight.y;
        } else {
            fixedSum.y += childData->WidgetMinimumSize.y;
        }
    }

    v2i weightedSize = {
        parentData->WidgetSize.x - (parentData->WidgetPadding.x*2) - fixedSum.x,
        parentData->WidgetSize.y - (parentData->WidgetPadding.y*2) - fixedSum.y
    };
    v2i position = {parentData->WidgetPadding.x, parentData->WidgetPadding.y};

    for(auto i = 0; i < numChildWidgets; ++i) {
        auto childWidget = childWidgets[i];

        if(!HasComponent(childWidget, ComponentOf_Widget())) continue;

        auto childData = GetWidgetData(childWidget);

        SetPosition2D(childWidget, {
            (float)position.x,
            (float)position.y
        });
        SetDistance2D(childWidget, -1.0f);

        v2i newSize;
        if(childData->WidgetWeight.x == 0.0f) {
            newSize.x = childData->WidgetMinimumSize.x;
        } else {
            newSize.x = s32(weightedSize.x * (childData->WidgetWeight.x / weightSum.x));
        }

        if(childData->WidgetWeight.y == 0.0f) {
            newSize.y = childData->WidgetMinimumSize.y;
        } else {
            newSize.y = s32(weightedSize.y * (childData->WidgetWeight.y / weightSum.y));
        }

        if(parentData->WidgetChildLayout == WidgetChildLayout_Horizontal) {
            position.x += newSize.x;
            newSize.y = parentData->WidgetSize.y - (parentData->WidgetPadding.y*2);
        } else {
            position.y += newSize.y;
            newSize.x = parentData->WidgetSize.x - (parentData->WidgetPadding.x*2);
        }

        SetWidgetSize(childWidget, newSize);
    }
}

static void UpdateWidgetBounds(Entity widget) {
    auto widgetSize = GetWidgetSize(widget);
    auto globalMat = GetTransformGlobalMatrix(widget);

    v4f localMin = { 0.0f, 0.0f, 0.0f, 1.0f };
    v4f localMax = { (float)widgetSize.x, (float)widgetSize.y, 0.0f, 1.0f };

    v4f globalMin, globalMax;

    glm_mat4_mulv((vec4*)&globalMat, &localMin.x, &globalMin.x);
    glm_mat4_mulv((vec4*)&globalMat, &localMax.x, &globalMax.x);

    SetRenderableAABBMin(widget, {globalMin.x, globalMin.y, globalMin.z});
    SetRenderableAABBMax(widget, {globalMax.x, globalMax.y, globalMax.z});
}

LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    if(HasComponent(newOwner, ComponentOf_Widget())) {
        UpdateChildrenLayout(newOwner);
    }
}

LocalFunction(OnChildSizeChanged, void, Entity entity) {
    auto owner = GetOwner(entity);
    if(HasComponent(owner, ComponentOf_Widget())) {
        UpdateChildrenLayout(owner);
    }
}

LocalFunction(OnWidgetSizeChanged, void, Entity widget, v2i oldSize, v2i newSize) {
    UpdateChildrenLayout(widget);
    UpdateWidgetBounds(widget);
}

LocalFunction(OnTransformGlobalChanged, void, Entity widget) {
    if(HasComponent(widget, ComponentOf_Widget())) {
        UpdateWidgetBounds(widget);
    }
}

LocalFunction(RebuildWidgetMesh, void, Entity mesh) {
    auto borderWidth = GetWidgetMeshFixedBorderWidth(mesh);
    auto texture = GetWidgetMeshTexture(mesh);
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

LocalFunction(OnValidate, void, Entity component) {
    for_entity(mesh, meshData, WidgetMesh, {
        if(IsDirty(mesh)) {
            RebuildWidgetMesh(mesh);
        }
    });
}

LocalFunction(OnWidgetMeshAdded, void, Entity component, Entity entity) {
    SetNumMeshSubMeshes(entity, 1);
    auto name = GetUuid(GetMeshSubMeshes(entity, NULL)[0]);
    Invalidate(entity);
}

LocalFunction(OnWidgetStateChanged, void, Entity widget) {
    auto data = GetInteractableWidgetData(widget);

    auto subMesh = data->WidgetEnabledSubMesh;

    if(data->WidgetFocused && IsEntityValid(data->WidgetFocusedSubMesh)) {
        subMesh = data->WidgetFocusedSubMesh;
    }

    if(data->WidgetHovered && IsEntityValid(data->WidgetHoveredSubMesh)) {
        subMesh = data->WidgetHoveredSubMesh;
    }

    if(data->WidgetClicked && IsEntityValid(data->WidgetClickedSubMesh)) {
        subMesh = data->WidgetClickedSubMesh;
    }

    if(data->WidgetDisabled && IsEntityValid(data->WidgetDisabledSubMesh)) {
        subMesh = data->WidgetDisabledSubMesh;
    }

    if(IsEntityValid(subMesh)) {
        SetRenderableSubMesh(widget, subMesh);
    }
}

BeginUnit(Widget)
    BeginEnum(WidgetChildLayout, false)
        RegisterFlag(WidgetChildLayout_Manual)
        RegisterFlag(WidgetChildLayout_Horizontal)
        RegisterFlag(WidgetChildLayout_Vertical)
    EndEnum()

    BeginComponent(Widget)
        RegisterBase(Renderable)
        RegisterProperty(v2i, WidgetMinimumSize)
        RegisterProperty(v2i, WidgetSize)
        RegisterProperty(v2f, WidgetWeight)
        RegisterPropertyEnum(u8, WidgetChildLayout, WidgetChildLayout)
        RegisterProperty(v2i, WidgetPadding)
        RegisterProperty(v2i, WidgetSpacing)
    EndComponent()

    BeginComponent(InteractableWidget)
        RegisterBase(Widget)
        RegisterProperty(v2i, WidgetInteractionPoint)
        RegisterProperty(bool, WidgetHovered)
        RegisterProperty(bool, WidgetFocused)
        RegisterProperty(bool, WidgetClicked)
        RegisterProperty(bool, WidgetDisabled)
        RegisterReferenceProperty(SubMesh, WidgetHoveredSubMesh)
        RegisterReferenceProperty(SubMesh, WidgetFocusedSubMesh)
        RegisterReferenceProperty(SubMesh, WidgetClickedSubMesh)
        RegisterReferenceProperty(SubMesh, WidgetDisabledSubMesh)
        RegisterReferenceProperty(SubMesh, WidgetEnabledSubMesh)
    EndComponent()

    BeginComponent(WidgetMesh)
        RegisterBase(MeshBuilder)
        RegisterReferenceProperty(SubTexture2D, WidgetMeshTexture)
        RegisterProperty(u16, WidgetMeshFixedBorderWidth)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMinimumSize()), OnChildSizeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetWeight()), OnChildSizeChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetSize()), OnWidgetSizeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetPadding()), OnWidgetSizeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetSpacing()), OnWidgetSizeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetChildLayout()), OnWidgetSizeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TransformGlobalMatrix()), OnTransformGlobalChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshFixedBorderWidth()), Invalidate, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetMeshTexture()), Invalidate, 0)
    RegisterSubscription(EventOf_Validate(), OnValidate, ComponentOf_Mesh())

    RegisterSubscription(EventOf_EntityComponentAdded(), OnWidgetMeshAdded, ComponentOf_WidgetMesh())

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetHovered()), OnWidgetStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetFocused()), OnWidgetStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClicked()), OnWidgetStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetDisabled()), OnWidgetStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetHoveredSubMesh()), OnWidgetStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetFocusedSubMesh()), OnWidgetStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClickedSubMesh()), OnWidgetStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetDisabledSubMesh()), OnWidgetStateChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetEnabledSubMesh()), OnWidgetStateChanged, 0)
EndUnit()
