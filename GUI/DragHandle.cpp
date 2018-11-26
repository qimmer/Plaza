//
// Created by Kim on 04-11-2018.
//

#include "DragHandle.h"
#include "Widget.h"
#include <Input/InputContext.h>
#include <Scene/Transform.h>

LocalFunction(OnWidgetClickedChanged, void, Entity widget, bool oldClicked, bool newClicked) {
    auto dragData = GetDragHandleData(widget);
    if(dragData && IsEntityValid(dragData->DragHandleTarget)) {
        auto interactionPoint = GetWidgetInteractionPoint(widget);
        auto pos = GetPosition2D(dragData->DragHandleTarget);
        SetDragHandleInteractionStart(widget, interactionPoint);
        SetDragHandlePositionStart(widget, pos);

        SetDragHandleDragging(widget, newClicked);
    }

    auto resizeData = GetResizeHandleData(widget);
    if(resizeData) {
        SetResizeHandleResizing(widget, newClicked);
    }
}

LocalFunction(OnWidgetInteractionPointChanged, void, Entity widget, v2i oldPoint, v2i newPoint) {
    auto dragHandleData = GetDragHandleData(widget);

    if(dragHandleData && dragHandleData->DragHandleDragging && IsEntityValid(dragHandleData->DragHandleTarget)) {
        SetPosition2D(dragHandleData->DragHandleTarget, {
            dragHandleData->DragHandlePositionStart.x + (newPoint.x - dragHandleData->DragHandleInteractionStart.x),
            dragHandleData->DragHandlePositionStart.y + (newPoint.y - dragHandleData->DragHandleInteractionStart.y)
        });
    }

    auto resizeHandleData = GetResizeHandleData(widget);

    if(resizeHandleData && resizeHandleData->ResizeHandleResizing && IsEntityValid(resizeHandleData->ResizeHandleTarget)) {
        auto size = GetWidgetSize(resizeHandleData->ResizeHandleTarget);
        size.x += (newPoint.x - oldPoint.x);
        size.y += (newPoint.y - oldPoint.y);
        SetWidgetSize(resizeHandleData->ResizeHandleTarget, size);
    }
}

BeginUnit(DragHandle)
    BeginComponent(DragHandle)
        RegisterBase(InteractableWidget)
        RegisterProperty(bool, DragHandleDragging)
        RegisterReferenceProperty(Widget, DragHandleTarget)
        RegisterProperty(v2f, DragHandlePositionStart)
        RegisterProperty(v2i, DragHandleInteractionStart)
    EndComponent()
    BeginComponent(ResizeHandle)
        RegisterBase(InteractableWidget)
        RegisterProperty(bool, ResizeHandleResizing)
        RegisterReferenceProperty(Widget, ResizeHandleTarget)
        RegisterProperty(v2i, ResizeHandleSizeStart)
        RegisterProperty(v2i, ResizeHandleInteractionStart)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClicked()), OnWidgetClickedChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetInteractionPoint()), OnWidgetInteractionPointChanged, 0)
EndUnit()

