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
        const Entity* childWidgets;
        Entity singleChild;
        if(GetPropertyKind(property) == PropertyKind_Array) {
			auto& array = GetArrayPropertyElements(property, layout);
            childWidgets = array.data();
			numChildWidgets = array.size();
        } else {
            singleChild = GetPropertyValue(property, layout).as_Entity;
            childWidgets = &singleChild;
            numChildWidgets = 1;
        }

        for(auto i = 0; i < numChildWidgets; ++i) {
            auto childWidget = childWidgets[i];

            if(GetHidden(childWidget)) continue;

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


    auto scrollOffset = GetLayoutScrollOffset(layout);

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
            const Entity *childWidgets;
            Entity singleChild;
            if(GetPropertyKind(property) == PropertyKind_Array) {
				auto& array = GetArrayPropertyElements(property, layout);
                childWidgets = array.data();
				numChildWidgets = array.size();
            } else {
                singleChild = GetPropertyValue(property, layout).as_Entity;
                childWidgets = &singleChild;
                numChildWidgets = 1;
            }

            for(auto i = 0; i < numChildWidgets; ++i) {
                auto childWidget = childWidgets[i];
                if(!childWidget) continue;

                auto childSize = GetSize2D(childWidget);
                auto childWeight = GetLayoutChildWeight(childWidget);

                if(GetHidden(childWidget)) continue;

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

    for_children(ordering, LayoutChildOrder, layout) {
        auto property = GetLayoutChildOrderingProperty(ordering);
        if(!HasComponent(property, ComponentOf_Property())) continue;

		u32 numChildWidgets = 0;
		const Entity *childWidgets;
		Entity singleChild;
		if (GetPropertyKind(property) == PropertyKind_Array) {
			auto& array = GetArrayPropertyElements(property, layout);
			childWidgets = array.data();
			numChildWidgets = array.size();
		}
		else {
			singleChild = GetPropertyValue(property, layout).as_Entity;
			childWidgets = &singleChild;
			numChildWidgets = 1;
		}

        for (auto j = 0; j < numChildWidgets; ++j) {
            auto childWidget = childWidgets[j];
            if(!childWidget) continue;

            auto size = GetSize2D(childWidget);

            if(GetHidden(childWidget)) continue;

            auto localLayer = (float)j / (numChildWidgets + 1);
			
			auto depth = -GetWidgetDepthOrder(childWidget) - localLayer;

            SetPosition3D(childWidget, {
                    (float) position.x + scrollOffset.x,
                    (float) position.y + scrollOffset.y,
					depth
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

LocalFunction(OnScrollOffsetChanged, void, Entity layout, v2i oldOffset, v2i newOffset) {
    for_children(ordering, LayoutChildOrder, layout) {
        auto property = GetLayoutChildOrderingProperty(ordering);
        if(!HasComponent(property, ComponentOf_Property())) continue;

        u32 numChildWidgets = 0;
        const Entity *childWidgets;
        Entity singleChild;
        if (GetPropertyKind(property) == PropertyKind_Array) {
            auto& array = GetArrayPropertyElements(property, layout);
            childWidgets = array.data();
            numChildWidgets = array.size();
        }
        else {
            singleChild = GetPropertyValue(property, layout).as_Entity;
            childWidgets = &singleChild;
            numChildWidgets = 1;
        }

        for (auto j = 0; j < numChildWidgets; ++j) {
            auto childWidget = childWidgets[j];
            auto position = GetPosition2D(childWidget);

            position.x += newOffset.x - oldOffset.x;
            position.y += newOffset.y - oldOffset.y;

            SetPosition2D(childWidget, position);
        }
    }
}

LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    Shrink(entity);
    Shrink(oldOwner);
    Shrink(newOwner);
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

LocalFunction(OnRect2DRemoved, void, Entity component, Entity entity) {
    auto owner = GetOwner(entity);
	if (HasComponent(owner, ComponentOf_Layout())) {
        Shrink(owner);
		ExpandChildLayouts(owner);
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
    BeginComponent(ScrollableLayout)
        RegisterBase(Layout)
        RegisterProperty(v2i, LayoutScrollOffset)
    EndComponent()
    BeginComponent(LayoutChild)
        RegisterProperty(v2f, LayoutChildWeight)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutChildWeight()), OnWeightChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Size2D()), OnSize2DChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutScrollOffset()), OnScrollOffsetChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetState()), OnWeightChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetDepthOrder()), OnWidgetDepthOrderChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Hidden()), OnWeightChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutMode()), OnLayoutModeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutSpacing()), OnLayoutModeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutPadding()), OnLayoutModeChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_LayoutChildOrderingProperty()), OnLayoutChildOrderingChanged, 0)
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnRect2DRemoved, ComponentOf_Ownership())
EndUnit()
