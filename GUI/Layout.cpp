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
#include <Foundation/Visibility.h>

static v2i CalculateMinimumSize(Entity layout) {
    if(!HasComponent(layout, ComponentOf_Layout())) return GetRect2D(layout).Size2D;

    auto layoutData = GetLayout(layout);
    auto padding = layoutData.LayoutPadding;
    auto spacing = layoutData.LayoutSpacing;
    auto layoutMode = layoutData.LayoutMode;

    v2i childrenSize = {
        padding.x + padding.z,
        padding.y + padding.w,
    };

    for(auto ordering : layoutData.LayoutChildOrder) {
        auto property = GetLayoutChildOrdering(ordering).LayoutChildOrderingProperty;
        auto propertyData = GetProperty(property);

        ChildArray arr;
        memset(&arr, 0, sizeof(ChildArray));

        if(propertyData.PropertyType == TypeOf_ChildArray) {
            arr = GetPropertyValue(property, layout).as_ChildArray;
        } else if(propertyData.PropertyType == TypeOf_Entity) {
            arr.SetSize(1);
            arr[0] = GetPropertyValue(property, layout).as_Entity;
        } else {
            continue;
        }

        for(auto i = 0; i < arr.GetSize(); ++i) {
            auto childWidget = arr[i];

            if(GetVisibility(childWidget).HierarchiallyHidden) continue;

            v2i childSize = GetRect2D(childWidget).Size2D;

            switch(layoutMode) {
                case LayoutMode_Vertical:
                    childrenSize.x = Max(childrenSize.x, childSize.x + padding.x + padding.z);
                    childrenSize.y += childSize.y;

                    // Add spacing before next element
                    if(i != (arr.GetSize() - 1)) {
                        childrenSize.y += spacing.y;
                    }
                    break;

                case LayoutMode_Horizontal:
                    childrenSize.x += childSize.x;
                    childrenSize.y = Max(childrenSize.y, childSize.y + padding.y + padding.w);

                    // Add spacing before next element
                    if(i != (arr.GetSize() - 1)) {
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

    auto widgetMesh = GetRenderable(layout).RenderableSubMesh;
    if(HasComponent(widgetMesh, ComponentOf_SubMesh())) {
        widgetMesh = GetOwnership(widgetMesh).Owner;
    }

    auto widgetBorderWidth = GetWidgetMesh(widgetMesh).WidgetMeshFixedBorderWidth;

    // Widgets cannot be smaller than it's borders
    childrenSize.x = Max(childrenSize.x, widgetBorderWidth * 2);
    childrenSize.y = Max(childrenSize.y, widgetBorderWidth * 2);

    return childrenSize;
}

static void Shrink(Entity layout) {
    /*for(auto ordering : layoutData.LayoutChildOrder) {
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
        auto weight = GetLayoutChild(layout).LayoutChildWeight;
        auto minimumSize = CalculateMinimumSize(layout);

        auto size = GetRect2D(layout).Size2D;
        if(weight.x == 0.0f) {
            size.x = minimumSize.x;
            sizeChanged = true;
        }
        if(weight.y == 0.0f) {
            size.y = minimumSize.y;
            sizeChanged = true;
        }

        if(sizeChanged) {
            SetRect2D(layout, {size});
        }
    }
}

static void ExpandChildLayouts(Entity layout) {
    auto parentData = GetLayout(layout);

    auto scrollOffset = GetScrollableLayout(layout).LayoutScrollOffset;

    auto parentSize = GetRect2D(layout).Size2D;
    auto minimumSize = CalculateMinimumSize(layout);

    if(!HasComponent(layout, ComponentOf_LayoutChild())) {
        SetRect2D(layout, {minimumSize});
        parentSize = minimumSize;
    }

    float weightSum = 0.0f;
    s32 fixedSum = 0;
    {
        for(auto ordering : parentData.LayoutChildOrder) {
            auto property = GetLayoutChildOrdering(ordering).LayoutChildOrderingProperty;
            auto propertyData = GetProperty(property);

            ChildArray arr;
            memset(&arr, 0, sizeof(ChildArray));

            if(propertyData.PropertyType == TypeOf_ChildArray) {
                arr = GetPropertyValue(property, layout).as_ChildArray;
            } else if(propertyData.PropertyType == TypeOf_Entity) {
                arr.SetSize(1);
                arr[0] = GetPropertyValue(property, layout).as_Entity;
            } else {
                continue;
            }

            for(auto i = 0; i < arr.GetSize(); ++i) {
                auto childWidget = arr[i];
                if(!childWidget) continue;

                auto childSize = GetRect2D(childWidget).Size2D;
                auto childWeight = GetLayoutChild(childWidget).LayoutChildWeight;

                if(GetVisibility(childWidget).HierarchiallyHidden) continue;

                if(parentData.LayoutMode == LayoutMode_Horizontal) {
                    if(childWeight.x > 0.0f) {
                        weightSum += childWeight.x;
                    } else {
                        fixedSum += childSize.x;
                    }
                }

                if(parentData.LayoutMode == LayoutMode_Vertical) {
                    if(childWeight.y > 0.0f) {
                        weightSum += childWeight.y;
                    } else {
                        fixedSum += childSize.y;
                    }
                }
            }
        }
    }


    float weightedSize = parentData.LayoutMode == LayoutMode_Horizontal
            ? (parentSize.x - (parentData.LayoutPadding.x + parentData.LayoutPadding.z) - (float)fixedSum)
            : (parentSize.y - (parentData.LayoutPadding.y + parentData.LayoutPadding.w) - (float)fixedSum);

    v2i position = {parentData.LayoutPadding.x, parentData.LayoutPadding.y};

    auto layer = 0;
    auto owner = GetOwnership(layout).Owner;
    while(owner) {
        layer++;
        owner = GetOwnership(owner).Owner;
    }

    for(auto ordering : parentData.LayoutChildOrder) {
        auto property = GetLayoutChildOrdering(ordering).LayoutChildOrderingProperty;
        auto propertyData = GetProperty(property);

        ChildArray arr;
        memset(&arr, 0, sizeof(ChildArray));

        if(propertyData.PropertyType == TypeOf_ChildArray) {
            arr = GetPropertyValue(property, layout).as_ChildArray;
        } else if(propertyData.PropertyType == TypeOf_Entity) {
            arr.SetSize(1);
            arr[0] = GetPropertyValue(property, layout).as_Entity;
        } else {
            continue;
        }

        for(auto i = 0; i < arr.GetSize(); ++i) {
            auto childWidget = arr[i];
            if(!childWidget) continue;

            auto size = GetRect2D(childWidget).Size2D;

            if(GetVisibility(childWidget).HierarchiallyHidden) continue;

            auto localLayer = (float)i / (arr.GetSize() + 1);
			
			auto depth = -GetWidget(childWidget).WidgetDepthOrder - localLayer;

			auto transformData = GetTransform(childWidget);
            transformData.Position3D = {
                (float) position.x + scrollOffset.x,
                (float) position.y + scrollOffset.y,
                depth
            };
            SetTransform(childWidget, transformData);

            auto childWeight = GetLayoutChild(childWidget).LayoutChildWeight;

            v2i newSize = size;
            switch(parentData.LayoutMode) {
                case LayoutMode_Vertical:
                    newSize.x = (childWeight.x > 0.0f) ? parentSize.x - (parentData.LayoutPadding.x + parentData.LayoutPadding.z) : size.x;
                    newSize.y = (childWeight.y > 0.0f) ? s32(weightedSize * (childWeight.y / weightSum)) : size.y;

                    position.y += newSize.y;
                    break;
                case LayoutMode_Horizontal:
                    newSize.x = (childWeight.x > 0.0f) ? s32(weightedSize * (childWeight.x / weightSum)) : size.x;
                    newSize.y = (childWeight.y > 0.0f) ? parentSize.y - (parentData.LayoutPadding.y + parentData.LayoutPadding.w) : size.y;

                    position.x += newSize.x;
                    break;
                case LayoutMode_Stacked:
                    position.x = parentData.LayoutPadding.x;
                    position.y = parentData.LayoutPadding.y;
                    newSize.x = parentSize.x - (parentData.LayoutPadding.x + parentData.LayoutPadding.z);
                    newSize.y = parentSize.y - (parentData.LayoutPadding.y + parentData.LayoutPadding.w);
                    break;
            }

            if(HasComponent(childWidget, ComponentOf_LayoutChild())) {
                SetRect2D(childWidget, {newSize});
            }
        }
    }
}

static void OnScrollableLayoutChanged(Entity layout, const ScrollableLayout& oldData, const ScrollableLayout& newData) {
    auto layoutData = GetLayout(layout);
    for(auto ordering : layoutData.LayoutChildOrder) {
        auto property = GetLayoutChildOrdering(ordering).LayoutChildOrderingProperty;
        if(!HasComponent(property, ComponentOf_Property())) continue;

        auto propertyData = GetProperty(property);

        ChildArray arr;
        memset(&arr, 0, sizeof(ChildArray));

        if(propertyData.PropertyType == TypeOf_ChildArray) {
            arr = GetPropertyValue(property, layout).as_ChildArray;
        } else if(propertyData.PropertyType == TypeOf_Entity) {
            arr.SetSize(1);
            arr[0] = GetPropertyValue(property, layout).as_Entity;
        } else {
            continue;
        }

        for(auto i = 0; i < arr.GetSize(); ++i) {
            auto childWidget = arr[i];
            auto transformData = GetTransform(childWidget);

            transformData.Position2D.x += newData.LayoutScrollOffset.x - oldData.LayoutScrollOffset.x;
            transformData.Position2D.y += newData.LayoutScrollOffset.y - oldData.LayoutScrollOffset.y;

            SetTransform(childWidget, transformData);
        }
    }
}

static void OnOwnershipChanged(Entity entity, const Ownership& oldData, const Ownership& newData) {
    if(HasComponent(entity, ComponentOf_Rect2D())) {
        Shrink(entity);
        Shrink(oldData.Owner);
        Shrink(newData.Owner);
        ExpandChildLayouts(oldData.Owner);
        ExpandChildLayouts(newData.Owner);
    }
}

static void OnLayoutChildChanged(Entity entity, const LayoutChild& oldData, const LayoutChild& newData) {
    auto owner = GetOwnership(entity).Owner;
    Shrink(entity);
    ExpandChildLayouts(owner);
}

static void OnLayoutChanged(Entity entity, const Layout& oldData, const Layout& newData) {
    Shrink(entity);
    ExpandChildLayouts(entity);
}

static void OnLayoutChildOrderingChanged(Entity entity, const LayoutChildOrdering& oldData, const LayoutChildOrdering& newData) {
    auto layout = GetOwnership(entity).Owner;
    Shrink(layout);
    ExpandChildLayouts(layout);
}

static void OnWidgetChanged(Entity entity, const Widget& oldData, const Widget& newData) {
    if(newData.WidgetDepthOrder != oldData.WidgetDepthOrder) {
        auto layout = GetOwnership(entity).Owner;
        Shrink(layout);
        ExpandChildLayouts(layout);
    }
}

static void OnRect2DChanged(Entity widget, const Rect2D& oldData, const Rect2D& newData) {
    auto owner = GetOwnership(widget).Owner;

    Shrink(owner);

    if(HasComponent(owner, ComponentOf_Layout())) {
        ExpandChildLayouts(owner);
    }

    if(HasComponent(widget, ComponentOf_Layout())) {
        ExpandChildLayouts(widget);
    }
}

static void OnVisibilityChanged(Entity entity, const Visibility& oldVisibility, const Visibility& newVisibility) {
    auto layout = GetOwnership(entity).Owner;
    Shrink(layout);
    ExpandChildLayouts(layout);
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
    BeginComponent(ScrollableLayout)
        RegisterBase(Layout)
        RegisterProperty(v2i, LayoutScrollOffset)
    EndComponent()
    BeginComponent(LayoutChild)
        RegisterProperty(v2f, LayoutChildWeight)
    EndComponent()

    RegisterSystem(OnLayoutChildChanged, ComponentOf_LayoutChild())
    RegisterSystem(OnOwnershipChanged, ComponentOf_Ownership())
    RegisterSystem(OnRect2DChanged, ComponentOf_Rect2D())
    RegisterSystem(OnScrollableLayoutChanged, ComponentOf_ScrollableLayout())
    RegisterSystem(OnWidgetChanged, ComponentOf_Widget())
    RegisterSystem(OnVisibilityChanged, ComponentOf_Visibility())
    RegisterSystem(OnLayoutChanged, ComponentOf_Layout())
EndUnit()
