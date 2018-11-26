//
// Created by Kim on 01-11-2018.
//

#include "GuiPickRay.h"
#include "Widget.h"
#include <Scene/Ray.h>
#include <Input/InputContext.h>
#include <Input/Key.h>
#include <Rendering/SceneRenderer.h>
#include <Scene/Transform.h>

LocalFunction(OnPickedPointChanged, void, Entity ray, v3f oldPoint, v3f newPoint) {
    if(!HasComponent(ray, ComponentOf_GuiPickRay())) {
        return;
    }

    auto widget = GetTraceRayRenderable(ray);
    if(HasComponent(widget, ComponentOf_InteractableWidget())) {
        auto widgetPoint = TransformPoint(0, widget, newPoint);
        SetWidgetInteractionPoint(widget, {(s32)widgetPoint.x, (s32)widgetPoint.y});
    }
    widget = GetGuiPickRayPickedWidget(ray);
    if(HasComponent(widget, ComponentOf_InteractableWidget())) {
        auto widgetPoint = TransformPoint(0, widget, newPoint);
        SetWidgetInteractionPoint(widget, {(s32)widgetPoint.x, (s32)widgetPoint.y});
    }
}

LocalFunction(OnPickedWidgetChanged, void, Entity ray, Entity oldWidget, Entity newWidget) {
    if(!HasComponent(ray, ComponentOf_GuiPickRay())) {
        return;
    }

    if(IsEntityValid(oldWidget)) {
        SetWidgetHovered(oldWidget, false);
    }

    if(IsEntityValid(newWidget)) {
        SetWidgetHovered(newWidget, true);
    }
}

LocalFunction(OnInputStateChanged, void, Entity inputState, float oldValue, float newValue) {
    auto ray = GetOwner(inputState);
    if(HasComponent(ray, ComponentOf_GuiPickRay())) {
        if(newValue > oldValue) { // Pressed
            auto widget = GetTraceRayRenderable(ray);
            if(IsEntityValid(widget)) {
                SetGuiPickRayPickedWidget(ray, widget);
                SetWidgetClicked(widget, true);
                SetWidgetFocused(widget, true);
            }
        } else { // Released
            auto widget = GetGuiPickRayPickedWidget(ray);
            if(IsEntityValid(widget)) {
                SetWidgetClicked(widget, false);
            }
        }
    }
}

LocalFunction(OnGuiPickRayAdded, void, Entity component, Entity entity) {
    SetTraceRayComponent(entity, ComponentOf_InteractableWidget());
}

BeginUnit(GuiPickRay)
    BeginComponent(GuiPickRay)
        RegisterBase(PickRay)
        RegisterBase(TraceRay)
        RegisterChildProperty(InputState, GuiPickRayClickState)
        RegisterReferenceProperty(Widget, GuiPickRayPickedWidget)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnGuiPickRayAdded, ComponentOf_GuiPickRay())
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TraceRayRenderable()), OnPickedWidgetChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_TraceRayPoint()), OnPickedPointChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_InputStateValue()), OnInputStateChanged, 0)
EndUnit()