//
// Created by Kim Johannsen on 30-03-2018.
//

#include <Rendering/Context.h>
#include <Rendering/RenderTarget.h>
#include <Foundation/AppLoop.h>
#include "MousePicker.h"
#include "ScenePicker.h"

struct MousePicker {
    Entity MousePickerContext;
    v4f MousePickerViewport;
};

DefineComponent(MousePicker)
    Dependency(ScenePicker)
    DefinePropertyReactive(Entity, MousePickerContext)
    DefinePropertyReactive(v4f, MousePickerViewport)
EndComponent()

DefineComponentPropertyReactive(MousePicker, v4f, MousePickerViewport)
DefineComponentPropertyReactive(MousePicker, Entity, MousePickerContext)

static void OnAppUpdate(double deltaTime) {
    for_entity(mousePicker, MousePicker) {
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
    Subscribe(AppUpdate, OnAppUpdate)
EndService()
