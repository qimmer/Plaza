//
// Created by Kim on 05/05/2018.
//

#include "UICamera.h"
#include <Scene/Camera.h>
#include <cglm/cglm.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/Context.h>

struct UICamera {
    UICamera() : UICameraPixelsPerUnit(1.0f) {}

    float UICameraPixelsPerUnit;
};

DefineComponent(UICamera)
    Dependency(Camera)
    DefinePropertyReactive(float, UICameraPixelsPerUnit)
EndComponent()

DefineComponentPropertyReactive(UICamera, float, UICameraPixelsPerUnit)

static void UpdateProjectionMatrix(Entity entity) {
    if(!HasUICamera(entity)) return;

    auto renderTarget = GetCameraRenderTarget(entity);
    if(!IsEntityValid(renderTarget) || !HasRenderTarget(renderTarget)) {
        return;
    }

    auto data = GetUICamera(entity);
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

static void OnCameraAspectChanged(Entity entity, float oldAspect, float newAspect) {
    UpdateProjectionMatrix(entity);
}

static void OnUICameraPixelsPerUnitChanged(Entity entity, float before, float after) {
    UpdateProjectionMatrix(entity);
}

static void OnCameraViewportChanged(Entity entity, v4f before, v4f after) {
    UpdateProjectionMatrix(entity);
}

static void OnCameraRenderTargetChanged(Entity entity, Entity before, Entity after) {
    UpdateProjectionMatrix(entity);
}

static void OnRenderTargetSizeChanged(Entity entity, v2i oldValue, v2i newValue) {
    for_entity(camera, UICamera) {
        if(GetCameraRenderTarget(camera) == entity) {
            UpdateProjectionMatrix(camera);
        }
    }
}

DefineService(UICamera)
    Subscribe(UICameraPixelsPerUnitChanged, OnUICameraPixelsPerUnitChanged)
    Subscribe(CameraViewportChanged, OnCameraViewportChanged)
    Subscribe(CameraRenderTargetChanged, OnCameraRenderTargetChanged)
    Subscribe(UICameraAdded, UpdateProjectionMatrix)
    Subscribe(RenderTargetSizeChanged, OnRenderTargetSizeChanged)
EndService()
