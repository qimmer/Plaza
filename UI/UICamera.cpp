//
// Created by Kim on 05/05/2018.
//

#include "UICamera.h"
#include <Scene/Camera.h>
#include <cglm/cglm.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/RenderContext.h>

struct UICamera {
    UICamera() : UICameraPixelsPerUnit(1.0f) {}

    float UICameraPixelsPerUnit;
};

BeginUnit(UICamera)
    BeginComponent(UICamera)
    RegisterBase(Camera)
    RegisterProperty(float, UICameraPixelsPerUnit)
EndComponent()
EndUnit()
(float, UICameraPixelsPerUnit)

static void UpdateProjectionMatrix(Entity entity) {
    if(!HasComponent(entity, ComponentOf_UICamera())) return;

    auto renderTarget = GetCameraRenderTarget(entity);
    if(!IsEntityValid(renderTarget) || !HasComponent(renderTarget, ComponentOf_RenderTarget())) {
        return;
    }

    auto data = GetUICameraData(entity);
    if(data->UICameraPixelsPerUnit <= FLT_EPSILON) {
        return;
    }

    auto viewport = GetCameraViewport(entity);
    v2f viewportSize = {(viewport.z - viewport.x), (viewport.w - viewport.y)};
    viewportSize.x *= GetRenderTargetSize(renderTarget).x;
    viewportSize.y *= GetRenderTargetSize(renderTarget).y;

    viewportSize.x /= data->UICameraPixelsPerUnit;
    viewportSize.y /= data->UICameraPixelsPerUnit;

    m4x4f projection;
    glm_ortho(0, viewportSize.x, viewportSize.y, 0, GetCameraNearClip(entity), GetCameraFarClip(entity), (vec4*)&projection);

    SetCameraProjectionMatrix(entity, projection);
}

LocalFunction(OnCameraAspectChanged, void, Entity entity, float oldAspect, float newAspect) {
    UpdateProjectionMatrix(entity);
}

LocalFunction(OnUICameraPixelsPerUnitChanged, void, Entity entity, float before, float after) {
    UpdateProjectionMatrix(entity);
}

LocalFunction(OnCameraViewportChanged, void, Entity entity, v4f before, v4f after) {
    UpdateProjectionMatrix(entity);
}

LocalFunction(OnCameraRenderTargetChanged, void, Entity entity, Entity before, Entity after) {
    UpdateProjectionMatrix(entity);
}

LocalFunction(OnRenderTargetSizeChanged, void, Entity entity, v2i oldValue, v2i newValue) {
    for_entity(camera, data, UICamera, {
        if(GetCameraRenderTarget(camera) == entity) {
            UpdateProjectionMatrix(camera);
        }
    }
}

DefineService(UICamera)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_UICameraPixelsPerUnit()), OnUICameraPixelsPerUnitChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CameraViewport()), OnCameraViewportChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_CameraRenderTarget()), OnCameraRenderTargetChanged, 0)
    RegisterSubscription(UICameraAdded, UpdateProjectionMatrix, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RenderTargetSize()), OnRenderTargetSizeChanged, 0)
EndService()
