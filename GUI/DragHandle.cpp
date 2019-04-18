//
// Created by Kim on 04-11-2018.
//

#include "DragHandle.h"
#include "Widget.h"
#include <Input/InputContext.h>
#include <Scene/Transform.h>

static void OnInteractableWidgetChanged(Entity widget, const InteractableWidget& oldData, const InteractableWidget& newData) {
    if (HasComponent(widget, ComponentOf_DragHandle())) {
        auto dragData = GetDragHandle(widget);

        if (IsEntityValid(dragData.DragHandleTarget)) {
            if (oldData.WidgetClicked != newData.WidgetClicked) {
                auto interactionPoint = GetInteractableWidget(widget).WidgetInteractionPoint;
                auto pos = GetTransform(dragData.DragHandleTarget).Position2D;

                dragData.DragHandleInteractionStart = interactionPoint;
                dragData.DragHandlePositionStart = pos;
                dragData.DragHandleDragging = newData.WidgetClicked;
                SetDragHandle(widget, dragData);
            }

            if (dragData.DragHandleDragging &&
                oldData.WidgetInteractionPoint.x != newData.WidgetInteractionPoint.x ||
                oldData.WidgetInteractionPoint.y != newData.WidgetInteractionPoint.y) {

                auto transformData = GetTransform(dragData.DragHandleTarget);
                transformData.Position2D = {
                        dragData.DragHandlePositionStart.x +
                        (newData.WidgetInteractionPoint.x - dragData.DragHandleInteractionStart.x),
                        dragData.DragHandlePositionStart.y +
                        (newData.WidgetInteractionPoint.y - dragData.DragHandleInteractionStart.y)
                };
                SetTransform(dragData.DragHandleTarget, transformData);
            }
        }
    }

    if (HasComponent(widget, ComponentOf_ResizeHandle())) {
        auto resizeData = GetResizeHandle(widget);

        if (IsEntityValid(resizeData.ResizeHandleTarget)) {
            if (oldData.WidgetClicked != newData.WidgetClicked) {
                resizeData.ResizeHandleResizing = newData.WidgetClicked;
                SetResizeHandle(widget, resizeData);
            }

            if (resizeData.ResizeHandleResizing &&
                oldData.WidgetInteractionPoint.x != newData.WidgetInteractionPoint.x ||
                oldData.WidgetInteractionPoint.y != newData.WidgetInteractionPoint.y) {
                auto size = GetRect2D(resizeData.ResizeHandleTarget).Size2D;
                size.x += (newData.WidgetInteractionPoint.x - oldData.WidgetInteractionPoint.x);
                size.y += (newData.WidgetInteractionPoint.y - oldData.WidgetInteractionPoint.y);
                SetRect2D(resizeData.ResizeHandleTarget, {size});
            }
        }
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

    RegisterSystem(OnInteractableWidgetChanged, ComponentOf_InteractableWidget())
EndUnit()

