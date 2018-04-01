//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Camera2D.h"
#include <Scene/Camera.h>
#include <cglm/cglm.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/Context.h>

struct Camera2D {
    float Camera2DPixelsPerUnit;
};

DefineComponent(Camera2D)
    Dependency(Camera)
    DefinePropertyReactive(float, Camera2DPixelsPerUnit)
EndComponent()

DefineComponentPropertyReactive(Camera2D, float, Camera2DPixelsPerUnit)

static void UpdateProjectionMatrix(Entity entity) {
    if(!HasCamera2D(entity)) return;

    auto renderTarget = GetCameraRenderTarget(entity);
    if(!IsEntityValid(renderTarget) || !HasRenderTarget(renderTarget)) {
        return;
    }

    auto data = GetCamera2D(entity);
    if(data->Camera2DPixelsPerUnit <= FLT_EPSILON) {
        return;
    }

    auto viewport = GetCameraViewport(entity);
    v2f viewportExtent = {(viewport.z - viewport.x) * 0.5f, (viewport.w - viewport.y) * 0.5f};
    viewportExtent.x *= GetRenderTargetSize(renderTarget).x;
    viewportExtent.y *= GetRenderTargetSize(renderTarget).y;

    viewportExtent.x /= data->Camera2DPixelsPerUnit;
    viewportExtent.y /= data->Camera2DPixelsPerUnit;

    m4x4f projection;
    glm_ortho(-viewportExtent.x, viewportExtent.x, -viewportExtent.y, viewportExtent.y, GetCameraNearClip(entity), GetCameraFarClip(entity), (vec4*)&projection);

    SetCameraProjectionMatrix(entity, projection);
}

static void OnCameraAspectChanged(Entity entity, float oldAspect, float newAspect) {
    UpdateProjectionMatrix(entity);
}

static void OnCamera2DPixelsPerUnitChanged(Entity entity, float before, float after) {
    UpdateProjectionMatrix(entity);
}

static void OnCameraViewportChanged(Entity entity, v4f before, v4f after) {
    UpdateProjectionMatrix(entity);
}

static void OnCameraRenderTargetChanged(Entity entity, Entity before, Entity after) {
    UpdateProjectionMatrix(entity);
}

static void OnRenderTargetSizeChanged(Entity entity, v2i oldValue, v2i newValue) {
    for_entity(camera, Camera2D) {
        if(GetCameraRenderTarget(camera) == entity) {
            UpdateProjectionMatrix(camera);
        }
    }
}

DefineService(Camera2D)
        Subscribe(Camera2DPixelsPerUnitChanged, OnCamera2DPixelsPerUnitChanged)
        Subscribe(CameraViewportChanged, OnCameraViewportChanged)
        Subscribe(CameraRenderTargetChanged, OnCameraRenderTargetChanged)
        Subscribe(Camera2DAdded, UpdateProjectionMatrix)
        Subscribe(RenderTargetSizeChanged, OnRenderTargetSizeChanged)
EndService()
