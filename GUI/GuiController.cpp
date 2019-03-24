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

LocalFunction(OnPickedPointChanged, void, Entity ray, v3f oldPoint, v3f newPoint) {
    auto controller = GetOwner(ray);
    if(!HasComponent(controller, ComponentOf_GuiController())) {
        return;
    }

    if(GetGuiControllerWidgetPicker(controller) == ray) {
        auto widget = GetTraceRayRenderable(ray);
        if(HasComponent(widget, ComponentOf_InteractableWidget())) {
            auto widgetPoint = TransformPoint(0, widget, newPoint);
            SetWidgetInteractionPoint(widget, {(s32)widgetPoint.x, (s32)widgetPoint.y});
        }
    }
}

LocalFunction(OnPickedWidgetChanged, void, Entity ray, Entity oldWidget, Entity newWidget) {
    auto controller = GetOwner(ray);
    if(!HasComponent(controller, ComponentOf_GuiController())) {
        return;
    }

    if(GetGuiControllerWidgetPicker(controller) == ray) {
        if(IsEntityValid(oldWidget)) {
            SetWidgetClicked(oldWidget, false);
            SetWidgetHovered(oldWidget, false);
        }

        if(IsEntityValid(newWidget)) {
            SetWidgetHovered(newWidget, true);
        }
    }
}

LocalFunction(OnGuiControllerLeftClickedChanged, void, Entity controller, bool oldValue, bool newValue) {
    auto ray = GetGuiControllerWidgetPicker(controller);

    if(newValue > oldValue) { // Pressed
        auto widget = GetTraceRayRenderable(ray);
        if(IsEntityValid(widget)) {
            SetWidgetClicked(widget, true);
            //SetWidgetFocused(widget, true);
        }
    } else { // Released
        auto widget = GetTraceRayRenderable(ray);
        if(IsEntityValid(widget)) {
            SetWidgetClicked(widget, false);
        }
    }
}

LocalFunction(OnGuiControllerScrollUpChanged, void, Entity controller, bool oldValue, bool newValue) {
    auto ray = GetGuiControllerScrollablePicker(controller);
    auto layout = GetTraceRayRenderable(ray);
    if(HasComponent(layout, ComponentOf_ScrollableLayout())) {
        auto offset = GetLayoutScrollOffset(layout);
        offset.y += 20;
        SetLayoutScrollOffset(layout, offset);
    }
}


LocalFunction(OnGuiControllerScrollDownChanged, void, Entity controller, bool oldValue, bool newValue) {
    auto ray = GetGuiControllerScrollablePicker(controller);
    auto layout = GetTraceRayRenderable(ray);
    if(HasComponent(layout, ComponentOf_ScrollableLayout())) {
        auto offset = GetLayoutScrollOffset(layout);
        offset.y -= 20;
        SetLayoutScrollOffset(layout, offset);
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
        RegisterChildProperty(TraceRay, GuiControllerWidgetPicker)
        RegisterChildProperty(TraceRay, GuiControllerScrollablePicker)

        ComponentTemplate({
            "GuiControllerWidgetPicker": {
                "TraceRayComponent": "Component.InteractableWidget"
            },
            "GuiControllerScrollablePicker": {
                "TraceRayComponent": "Component.ScrollableLayout"
            }
        })
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TraceRayRenderable()), OnPickedWidgetChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TraceRayPoint()), OnPickedPointChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_GuiControllerLeftClicked()), OnGuiControllerLeftClickedChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_GuiControllerScrollUp()), OnGuiControllerScrollUpChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_GuiControllerScrollDown()), OnGuiControllerScrollDownChanged, 0)
EndUnit()