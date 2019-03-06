//
// Created by Kim on 11-02-2019.
//

#include <Foundation/AppNode.h>
#include <Scene/Transform.h>
#include "Layout.h"
#include "Widget.h"
#include <Rendering/Renderable.h>
#include <Rendering/Mesh.h>
#include <Core/Algorithms.h>
#include <Json/NativeUtils.h>
#include <Scene/Scene.h>

static v2i CalculateMinimumSize(Entity layout) {
    if(!HasComponent(layout, ComponentOf_Layout())) return GetSize2D(layout);

    auto padding = GetLayoutPadding(layout);
    auto spacing = GetLayoutSpacing(layout);
    auto layoutMode = GetLayoutMode(layout);

    v2i childrenSize = {
        padding.x + padding.z,
        padding.y + padding.w,
    };

    for_children(ordering, LayoutChildOrder, layout) {
        auto property = GetLayoutChildOrderingProperty(ordering);
        if(!HasComponent(property, ComponentOf_Property())) continue;

        u32 numChildWidgets = 0;
        Entity *childWidgets;
        Entity singleChild;
        if(GetPropertyKind(property) == PropertyKind_Array) {
            childWidgets = GetArrayPropertyElements(property, layout, &numChildWidgets);
        } else {
            singleChild = GetPropertyValue(property, layout).as_Entity;
            childWidgets = &singleChild;
            numChildWidgets = 1;
        }

        for(auto i = 0; i < numChildWidgets; ++i) {
            auto childWidget = childWidgets[i];

            v2i childSize = GetSize2D(childWidget);

            switch(layoutMode) {
                case LayoutMode_Vertical:
                    childrenSize.x = Max(childrenSize.x, childSize.x + padding.x + padding.z);
                    childrenSize.y += childSize.y;

                    // Add spacing before next element
                    if(i != (numChildWidgets - 1)) {
                        childrenSize.y += spacing.y;
                    }
                    break;

                case LayoutMode_Horizontal:
                    childrenSize.x += childSize.x;
                    childrenSize.y = Max(childrenSize.y, childSize.y + padding.y + padding.w);

                    // Add spacing before next element
                    if(i != (numChildWidgets - 1)) {
                        childrenSize.x += spacing.x;
                    }
                    break;

                case LayoutMode_Stacked:
                    childrenSize.x = Max(childrenSize.x, childSize.x + padding.x + padding.z);
                    childrenSize.y = Max(childrenSize.y, childSize.y + padding.y + padding.w);
                    break;
            }
        }
    }

    auto widgetMesh = GetRenderableSubMesh(layout);
    if(HasComponent(widgetMesh, ComponentOf_SubMesh())) {
        widgetMesh = GetOwner(widgetMesh);
    }

    auto widgetBorderWidth = GetWidgetMeshFixedBorderWidth(widgetMesh);

    // Widgets cannot be smaller than it's borders
    childrenSize.x = Max(childrenSize.x, widgetBorderWidth * 2);
    childrenSize.y = Max(childrenSize.y, widgetBorderWidth * 2);

    return childrenSize;
}

static void Shrink(Entity layout) {
    /*for_children(ordering, LayoutChildOrder, layout) {
        auto property = GetLayoutChildOrderingProperty(ordering);
        if(!HasComponent(property, ComponentOf_Property())) continue;

        u32 numChildWidgets = 0;
        Entity *childWidgets;
        Entity singleChild;
        if(GetPropertyKind(property) == PropertyKind_Array) {
            childWidgets = GetArrayPropertyElements(property, layout, &numChildWidgets);
        } else {
            singleChild = GetPropertyValue(property, layout).as_Entity;
            childWidgets = &singleChild;
            numChildWidgets = 1;
        }

        for(auto i = 0; i < numChildWidgets; ++i) {
            auto childWidget = childWidgets[i];

            Shrink(childWidget);
        }
    }*/

    bool sizeChanged = false;
    if(HasComponent(layout, ComponentOf_LayoutChild())) {
        auto weight = GetLayoutChildWeight(layout);
        auto minimumSize = CalculateMinimumSize(layout);

        auto size = GetSize2D(layout);
        if(weight.x == 0.0f) {
            size.x = minimumSize.x;
            sizeChanged = true;
        }
        if(weight.y == 0.0f) {
            size.y = minimumSize.y;
            sizeChanged = true;
        }

        if(sizeChanged) {
            SetSize2D(layout, size);
        }
    }
}

static void ExpandChildLayouts(Entity layout) {
    auto parentData = GetLayoutData(layout);
    if(!parentData) return;

    auto parentSize = GetSize2D(layout);
    auto minimumSize = CalculateMinimumSize(layout);

    if(!HasComponent(layout, ComponentOf_LayoutChild())) {
        SetSize2D(layout, minimumSize);
        parentSize = minimumSize;
    }

    float weightSum = 0.0f;
    s32 fixedSum = 0;
    {
        for_children(ordering, LayoutChildOrder, layout) {
            auto property = GetLayoutChildOrderingProperty(ordering);
            if(!HasComponent(property, ComponentOf_Property())) continue;

            u32 numChildWidgets = 0;
            Entity *childWidgets;
            Entity singleChild;
            if(GetPropertyKind(property) == PropertyKind_Array) {
                childWidgets = GetArrayPropertyElements(property, layout, &numChildWidgets);
            } else {
                singleChild = GetPropertyValue(property, layout).as_Entity;
                childWidgets = &singleChild;
                numChildWidgets = 1;
            }

            for(auto i = 0; i < numChildWidgets; ++i) {
                auto childWidget = childWidgets[i];
                auto childSize = GetSize2D(childWidget);
                auto childWeight = GetLayoutChildWeight(childWidget);
                auto hiddenState = GetWidgetState(childWidget).z;

                if(parentData->LayoutMode == LayoutMode_Horizontal) {
                    if(childWeight.x > 0.0f) {
                        weightSum += childWeight.x;
                    } else {
                        fixedSum += childSize.x;
                    }
                }

                if(parentData->LayoutMode == LayoutMode_Vertical) {
                    if(childWeight.y > 0.0f) {
                        weightSum += childWeight.y;
                    } else {
                        fixedSum += childSize.y;
                    }
                }
            }
        }
    }


    float weightedSize = parentData->LayoutMode == LayoutMode_Horizontal
            ? (parentSize.x - (parentData->LayoutPadding.x + parentData->LayoutPadding.z) - (float)fixedSum)
            : (parentSize.y - (parentData->LayoutPadding.y + parentData->LayoutPadding.w) - (float)fixedSum);

    v2i position = {parentData->LayoutPadding.x, parentData->LayoutPadding.y};

    auto layer = 0;
    auto owner = GetOwner(layout);
    while(owner) {
        layer++;
        owner = GetOwner(owner);
    }

    auto layoutDistanceFactor = 1.0f / (float)(10 ^ layer);
    auto childDistanceFactor = 1.0f / (float)(10 ^ (layer + 1));

    for_children(ordering, LayoutChildOrder, layout) {
        auto property = GetLayoutChildOrderingProperty(ordering);
        if(!HasComponent(property, ComponentOf_Property())) continue;

        u32 numChildWidgets = 0;
        Entity *childWidgets;
        Entity singleChild;
        if(GetPropertyKind(property) == PropertyKind_Array) {
            childWidgets = GetArrayPropertyElements(property, layout, &numChildWidgets);
        } else {
            singleChild = GetPropertyValue(property, layout).as_Entity;
            childWidgets = &singleChild;
            numChildWidgets = 1;
        }

        for (auto j = 0; j < numChildWidgets; ++j) {
            auto childWidget = childWidgets[j];
            auto size = GetSize2D(childWidget);
            auto hiddenState = GetWidgetState(childWidget).z;
            auto localLayer = (float)j / numChildWidgets;

            SetPosition3D(childWidget, {
                    (float) position.x,
                    (float) position.y,
                    -GetWidgetDepthOrder(childWidget) -layoutDistanceFactor -localLayer
            });

            auto childWeight = GetLayoutChildWeight(childWidget);

            v2i newSize = size;
            switch(GetLayoutMode(layout)) {
                case LayoutMode_Vertical:
                    newSize.x = (childWeight.x > 0.0f) ? parentSize.x - (parentData->LayoutPadding.x + parentData->LayoutPadding.z) : size.x;
                    newSize.y = (childWeight.y > 0.0f) ? s32(weightedSize * (childWeight.y / weightSum)) : size.y;

                    position.y += newSize.y;
                    break;
                case LayoutMode_Horizontal:
                    newSize.x = (childWeight.x > 0.0f) ? s32(weightedSize * (childWeight.x / weightSum)) : size.x;
                    newSize.y = (childWeight.y > 0.0f) ? parentSize.y - (parentData->LayoutPadding.y + parentData->LayoutPadding.w) : size.y;

                    position.x += newSize.x;
                    break;
                case LayoutMode_Stacked:
                    position.x = parentData->LayoutPadding.x;
                    position.y = parentData->LayoutPadding.y;
                    newSize.x = parentSize.x - (parentData->LayoutPadding.x + parentData->LayoutPadding.z);
                    newSize.y = parentSize.y - (parentData->LayoutPadding.y + parentData->LayoutPadding.w);
                    break;
            }

            if(HasComponent(childWidget, ComponentOf_LayoutChild())) {
                SetSize2D(childWidget, newSize);
            }
        }
    }
}


LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    Shrink(entity);
    Shrink(oldOwner);
    ExpandChildLayouts(oldOwner);
    ExpandChildLayouts(newOwner);
}

LocalFunction(OnWeightChanged, void, Entity entity) {
    auto owner = GetOwner(entity);
    Shrink(entity);
    ExpandChildLayouts(owner);
}

LocalFunction(OnLayoutModeChanged, void, Entity entity) {
    Shrink(entity);
    ExpandChildLayouts(entity);
}

LocalFunction(OnLayoutChildOrderingChanged, void, Entity entity) {
    auto layout = GetOwner(entity);
    OnLayoutModeChanged(layout);
}

LocalFunction(OnWidgetDepthOrderChanged, void, Entity entity) {
    auto layout = GetOwner(entity);
    OnLayoutModeChanged(layout);
}

LocalFunction(OnSize2DChanged, void, Entity widget, v2i oldSize, v2i newSize) {
    auto owner = GetOwner(widget);

    Shrink(owner);

    if(HasComponent(owner, ComponentOf_Layout())) {
        ExpandChildLayouts(owner);
    }

    if(HasComponent(widget, ComponentOf_Layout())) {
        ExpandChildLayouts(widget);
    }
}

BeginUnit(Layout)
    BeginComponent(LayoutChildOrdering)
        RegisterReferenceProperty(Property, LayoutChildOrderingProperty)
    EndComponent()
    BeginEnum(LayoutMode, false)
        RegisterFlag(LayoutMode_Vertical)
        RegisterFlag(LayoutMode_Horizontal)
        RegisterFlag(LayoutMode_Stacked)
    EndEnum()
    BeginComponent(Layout)
        RegisterProperty(v4i, LayoutPadding)
        RegisterProperty(v2i, LayoutSpacing)
        RegisterPropertyEnum(u8, LayoutMode, LayoutMode)
        RegisterArrayProperty(LayoutChildOrdering, LayoutChildOrder)
    EndComponent()
    BeginComponent(LayoutChild)
        RegisterProperty(v2f, LayoutChildWeight)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutChildWeight()), OnWeightChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Size2D()), OnSize2DChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetState()), OnWeightChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetDepthOrder()), OnWidgetDepthOrderChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutMode()), OnLayoutModeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutSpacing()), OnLayoutModeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutPadding()), OnLayoutModeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutChildOrderingProperty()), OnLayoutChildOrderingChanged, 0)
EndUnit()
