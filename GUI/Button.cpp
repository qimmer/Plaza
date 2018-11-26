//
// Created by Kim on 04-11-2018.
//

#include "Button.h"
#include "Widget.h"
#include <Foundation/Invocation.h>

LocalFunction(OnWidgetClicked, void, Entity entity, bool oldValue, bool newValue) {
    if(!newValue && HasComponent(entity, ComponentOf_Button())) {
        Invoke(GetButtonAction(entity));
    }
}

BeginUnit(Button)
    BeginComponent(Button)
        RegisterChildProperty(Invocation, ButtonAction)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClicked()), OnWidgetClicked, 0)
EndUnit()