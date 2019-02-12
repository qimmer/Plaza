//
// Created by Kim on 11-02-2019.
//

#include <Foundation/AppNode.h>
#include <Scene/Transform.h>
#include "Layout.h"
#include "Widget.h"


static void UpdateChildrenLayout(Entity layout) {
    auto parentData = GetLayoutData(layout);
    if(!parentData) return;

    auto parentSize = GetSize2D(layout);

    u32 numChildWidgets = 0;
    auto childWidgets = GetChildren(layout, &numChildWidgets);

    v2f weightSum = {0.0f, 0.0f};
    v2f fixedSum = {0, 0};
    for(auto i = 0; i < numChildWidgets; ++i) {
        auto childWidget = childWidgets[i];

        auto childData = GetLayoutChildData(childWidget);
        if(!childData) continue;

        if(childData->LayoutChildWeight.x != 0.0f) {
            weightSum.x += childData->LayoutChildWeight.x;
        } else {
            fixedSum.x += childData->LayoutChildMinimumSize.x;
        }

        if(childData->LayoutChildWeight.y != 0.0f) {
            weightSum.y += childData->LayoutChildWeight.y;
        } else {
            fixedSum.y += childData->LayoutChildMinimumSize.y;
        }
    }

    v2f weightedSize = {
            parentSize.x - (parentData->LayoutPadding.x*2) - fixedSum.x,
            parentSize.y - (parentData->LayoutPadding.y*2) - fixedSum.y
    };
    v2f position = {parentData->LayoutPadding.x, parentData->LayoutPadding.y};

    for(auto i = 0; i < numChildWidgets; ++i) {
        auto childWidget = childWidgets[i];

        AddComponent(childWidget, ComponentOf_LayoutChild());
        auto childData = GetLayoutChildData(childWidget);

        SetPosition2D(childWidget, {
                (float)position.x,
                (float)position.y
        });
        SetDistance2D(childWidget, -1.0f);

        v2f newSize;
        if(childData->LayoutChildWeight.x == 0.0f) {
            newSize.x = childData->LayoutChildMinimumSize.x;
        } else {
            newSize.x = s32(weightedSize.x * (childData->LayoutChildWeight.x / weightSum.x));
        }

        if(childData->LayoutChildWeight.y == 0.0f) {
            newSize.y = childData->LayoutChildMinimumSize.y;
        } else {
            newSize.y = s32(weightedSize.y * (childData->LayoutChildWeight.y / weightSum.y));
        }

        auto childPadding = GetLayoutPadding(childWidget);

        if(HasComponent(layout, ComponentOf_HorizontalLayout())) {
            position.x += newSize.x;
            newSize.y = parentSize.y - (childPadding.y*2);
        }

        if(HasComponent(layout, ComponentOf_VerticalLayout())) {
            position.y += newSize.y;
            newSize.x = parentSize.x - (childPadding.x*2);
        }

        SetSize2D(childWidget, newSize);
    }
}


LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    if(HasComponent(newOwner, ComponentOf_Layout())) {
        UpdateChildrenLayout(newOwner);
    }
}

LocalFunction(OnChildSizeChanged, void, Entity entity) {
    auto owner = GetOwner(entity);
    UpdateChildrenLayout(owner);
}

LocalFunction(OnSize2DChanged, void, Entity widget) {
    if(HasComponent(widget, ComponentOf_Layout())) {
        UpdateChildrenLayout(widget);
    }
}

BeginUnit(Layout)
    BeginComponent(Layout)
        RegisterProperty(v4f, LayoutPadding)
        RegisterProperty(v2f, LayoutSpacing)
    EndComponent()
    BeginComponent(LayoutChild)
        RegisterProperty(v2f, LayoutChildMinimumSize)
        RegisterProperty(v2f, LayoutChildWeight)
    EndComponent()
    BeginComponent(VerticalLayout)
        RegisterBase(Layout)
    EndComponent()
    BeginComponent(HorizontalLayout)
        RegisterBase(Layout)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutChildMinimumSize()), OnChildSizeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutChildWeight()), OnChildSizeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Size2D()), OnSize2DChanged, 0)
EndUnit()
