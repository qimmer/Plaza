//
// Created by Kim Johannsen on 03-04-2018.
//

#include <Scene/MeshInstance.h>
#include <Scene2D/Transform2D.h>
#include "Widget.h"

DefineEvent(WidgetMouseDown, EntityHandler)
DefineEvent(WidgetMouseUp, EntityHandler)

DefineEnum(WidgetChildLayout, false)
    DefineFlag(WidgetChildLayout_Horizontal)
    DefineFlag(WidgetChildLayout_Vertical)
EndEnum()

DefineEnum(WidgetSizing, false)
    DefineFlag(WidgetSizing_Fixed)
    DefineFlag(WidgetSizing_Weighted)
EndEnum()

struct Widget {
    Widget() : WidgetWeight({1.0f, 1.0f}) {}

    v2i WidgetSize, WidgetMinimumSize;
    v2f WidgetWeight;
    u8 WidgetChildLayout, WidgetSizing;
};

DefineComponent(Widget)
    Dependency(MeshInstance)
    DefineProperty(v2i, WidgetMinimumSize)
    DefineProperty(v2i, WidgetSize)
    DefineProperty(v2f, WidgetWeight)
    DefinePropertyEnum(u8, WidgetChildLayout, WidgetChildLayout)
    DefinePropertyEnum(u8, WidgetSizing, WidgetSizing)
EndComponent()

DefineComponentPropertyReactive(Widget, v2i, WidgetSize)
DefineComponentPropertyReactive(Widget, v2i, WidgetMinimumSize)
DefineComponentPropertyReactive(Widget, v2f, WidgetWeight)
DefineComponentPropertyReactive(Widget, u8, WidgetChildLayout)
DefineComponentPropertyReactive(Widget, u8, WidgetSizing)

static void UpdateChildrenLayout(Entity parentWidget) {
    auto parentData = GetWidget(parentWidget);

    v2f weightSum = {0.0f, 0.0f};
    for_children(childWidget, parentWidget) {
        auto childData = GetWidget(childWidget);
        weightSum.x += childData->WidgetWeight.x;
        weightSum.y += childData->WidgetWeight.y;
    }

    v2i position = {0, 0};
    for_children(childWidget, parentWidget) {
        auto childData = GetWidget(childWidget);

        SetPosition2D(childWidget, {position.x, position.y});

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

static void OnWidgetSizeChanged(Entity widget, v2i oldSize, v2i newSize) {
    UpdateChildrenLayout(widget);
}

DefineService(Widget)
    Subscribe(WidgetSizeChanged, OnWidgetSizeChanged)
EndService()
