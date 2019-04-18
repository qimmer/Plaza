//
// Created by Kim on 01-11-2018.
//

#include "GuiController.h"
#include "Widget.h"
#include "Layout.h"
#include <Scene/Ray.h>
#include <Input/InputContext.h>
#include <Input/Key.h>
#include <Rendering/SceneRenderer.h>
#include <Scene/Transform.h>
#include <Json/NativeUtils.h>

static void OnTraceRayChanged(Entity entity, const TraceRay& oldData, const TraceRay& newData) {
    auto controller = GetOwnership(entity).Owner;
    if(!HasComponent(controller, ComponentOf_GuiController())) {
        return;
    }

    if(newData.TraceRayRenderable) {
        auto widget = newData.TraceRayRenderable;
        auto interactableWidgetData = GetInteractableWidget(widget);

        if(memcmp(&oldData.TraceRayPoint, &newData.TraceRayPoint, sizeof(v3f)) != 0) {
            auto widgetPoint = TransformPoint(0, widget, newData.TraceRayPoint);

            interactableWidgetData.WidgetInteractionPoint = {(s32)widgetPoint.x, (s32)widgetPoint.y};
            SetInteractableWidget(widget, interactableWidgetData);
        }

        if(oldData.TraceRayRenderable != newData.TraceRayRenderable) {
            if(IsEntityValid(oldData.TraceRayRenderable)) {
                auto oldWidgetData = GetInteractableWidget(oldData.TraceRayRenderable);
                oldWidgetData.WidgetClicked = false;
                oldWidgetData.WidgetHovered = false;
                SetInteractableWidget(oldData.TraceRayRenderable, oldWidgetData);
            }

            if(IsEntityValid(newData.TraceRayRenderable)) {
                interactableWidgetData.WidgetHovered = true;
                SetInteractableWidget(widget, interactableWidgetData);
            }
        }
    }
}

static void OnGuiControllerChanged(Entity entity, const GuiController& oldData, const GuiController& newData) {
    auto widget = GetTraceRay(newData.GuiControllerWidgetPicker).TraceRayRenderable;
    auto scrollableLayout = GetTraceRay(newData.GuiControllerScrollablePicker).TraceRayRenderable;

    // Clicked
    if(!oldData.GuiControllerLeftClicked && newData.GuiControllerLeftClicked) {
        if(IsEntityValid(widget)) {
            auto widgetData = GetInteractableWidget(widget);
            widgetData.WidgetClicked = true;
            SetInteractableWidget(widget, widgetData);
        }
    }

    // Released
    if(oldData.GuiControllerLeftClicked && !newData.GuiControllerLeftClicked) {
        if(IsEntityValid(widget)) {
            auto widgetData = GetInteractableWidget(widget);
            widgetData.WidgetClicked = false;
            SetInteractableWidget(widget, widgetData);
        }
    }

    if(!oldData.GuiControllerScrollUp && newData.GuiControllerScrollUp) {
        if(HasComponent(widget, ComponentOf_ScrollableLayout())) {
            auto scrollableLayoutData = GetScrollableLayout(scrollableLayout);
            scrollableLayoutData.LayoutScrollOffset.y += 20;
            SetScrollableLayout(scrollableLayout, scrollableLayoutData);
        }
    }

    if(!oldData.GuiControllerScrollDown && newData.GuiControllerScrollDown) {
        if(HasComponent(widget, ComponentOf_ScrollableLayout())) {
            auto scrollableLayoutData = GetScrollableLayout(scrollableLayout);
            scrollableLayoutData.LayoutScrollOffset.y -= 20;
            SetScrollableLayout(scrollableLayout, scrollableLayoutData);
        }
    }


}

BeginUnit(GuiController)
    BeginComponent(GuiController)
        RegisterProperty(bool, GuiControllerLeftClicked)
        RegisterProperty(bool, GuiControllerMiddleClicked)
        RegisterProperty(bool, GuiControllerRightClicked)
        RegisterProperty(bool, GuiControllerScrollUp)
        RegisterProperty(bool, GuiControllerScrollDown)
        RegisterProperty(s8, GuiControllerLastCharacter)
        BeginChildProperty(GuiControllerWidgetPicker)
            BeginChildComponent(TraceRay)
                data.TraceRayComponent = ComponentOf_InteractableWidget();
            EndChildComponent()
        EndChildProperty()
        BeginChildProperty(GuiControllerScrollablePicker)
            BeginChildComponent(TraceRay)
                data.TraceRayComponent = ComponentOf_ScrollableLayout();
            EndChildComponent()
        EndChildProperty()
    EndComponent()

    RegisterSystem(OnTraceRayChanged, ComponentOf_TraceRay())
    RegisterSystem(OnGuiControllerChanged, ComponentOf_GuiController())
EndUnit()