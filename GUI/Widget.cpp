//
// Created by Kim Johannsen on 03-04-2018.
//

#include <Foundation/AppNode.h>
#include <Scene/Transform.h>
#include <Scene/Transform2D.h>
#include <Rendering/Mesh.h>
#include <Rendering/Renderable.h>
#include <Foundation/Stream.h>
#include <Rendering/Uniform.h>
#include <Rendering/Material.h>
#include <Rendering/Texture.h>
#include <Rendering/Texture2D.h>
#include "Widget.h"

static void UpdateChildrenLayout(Entity parentWidget) {
    auto parentData = GetWidgetData(parentWidget);

    v2f weightSum = {0.0f, 0.0f};
    for_children(childWidget, Children, parentWidget, {
        if(!HasComponent(childWidget, ComponentOf_Widget())) continue;

        auto childData = GetWidgetData(childWidget);
        weightSum.x += childData->WidgetWeight.x;
        weightSum.y += childData->WidgetWeight.y;
    });

    v2i position = {0, 0};
    for_children(childWidget, Children, parentWidget, {
        if(!HasComponent(childWidget, ComponentOf_Widget())) continue;

        auto childData = GetWidgetData(childWidget);

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
    });
}

LocalFunction(OnWidgetSizeChanged, void, Entity widget, v2i oldSize, v2i newSize) {
    UpdateChildrenLayout(widget);
}

BeginUnit(Widget)
    BeginEnum(WidgetChildLayout, false)
    RegisterFlag(WidgetChildLayout_Horizontal)
    RegisterFlag(WidgetChildLayout_Vertical)
    EndEnum()

    BeginEnum(WidgetSizing, false)
    RegisterFlag(WidgetSizing_Fixed)
    RegisterFlag(WidgetSizing_Weighted)
    EndEnum()

    BeginComponent(Widget)
        RegisterBase(Renderable)
        RegisterProperty(v2i, WidgetMinimumSize)
        RegisterProperty(v2i, WidgetSize)
        RegisterProperty(v2f, WidgetWeight)
        RegisterPropertyEnum(u8, WidgetChildLayout, WidgetChildLayout)
        RegisterPropertyEnum(u8, WidgetSizing, WidgetSizing)
    EndComponent()

    BeginComponent(WidgetMesh)
        RegisterReferenceProperty(Texture2D, WidgetMeshTexture)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetSize()), OnWidgetSizeChanged, 0)
EndUnit()
