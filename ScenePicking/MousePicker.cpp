//
// Created by Kim Johannsen on 30-03-2018.
//

#include <Rendering/RenderContext.h>
#include <Rendering/RenderTarget.h>
#include <Foundation/AppLoop.h>
#include "MousePicker.h"
#include "ScenePicker.h"

struct MousePicker {
    Entity MousePickerContext;
    v4f MousePickerViewport;
};

BeginUnit(MousePicker)
    BeginComponent(MousePicker)
    RegisterBase(ScenePicker)
    RegisterProperty(Entity, MousePickerContext)
    RegisterProperty(v4f, MousePickerViewport)
EndComponent()
EndUnit()
(v4f, MousePickerViewport)
RegisterProperty(Entity, MousePickerContext)

LocalFunction(OnAppUpdate, void, double deltaTime) {
    for_entity(mousePicker, data, MousePicker) {
        auto context = GetMousePickerContext(mousePicker);
        if(IsEntityValid(context)) {
            auto cursor = GetCursorPosition(context, 0);
            auto contextSize = GetRenderTargetSize(context);
            auto viewport = GetMousePickerViewport(mousePicker);
            v2f normalized = {
                (float)cursor.x / contextSize.x,
                (float)cursor.y / contextSize.y
            };
            v2f local = {
                normalized.x / viewport.z + viewport.x,
                normalized.y / viewport.w + viewport.y,
            };
            SetScenePickerViewportLocation(mousePicker, local);
        }
    }
}

DefineService(MousePicker)
    RegisterSubscription(AppUpdate, OnAppUpdate, 0)
EndService()
