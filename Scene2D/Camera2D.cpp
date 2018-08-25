//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Camera2D.h"
#include <Scene/Camera.h>
#include <cglm/cglm.h>
#include <Rendering/RenderTarget.h>
#include <Rendering/RenderContext.h>

struct Camera2D {
    Camera2D() : Camera2DPixelsPerUnit(32.0f) {}

    float Camera2DPixelsPerUnit;
};

BeginUnit(Camera2D)
    BeginComponent(Camera2D)
    RegisterBase(Camera)
    RegisterProperty(float, Camera2DPixelsPerUnit)
EndComponent()
EndUnit()
(float, Camera2DPixelsPerUnit)

static void UpdateProjectionMatrix(Entity entity) {
    if(!HasComponent(entity, ComponentOf_Camera2D())) return;

    auto renderTarget = GetCameraRenderTarget(entity);
    if(!IsEntityValid(renderTarget) || !HasComponent(renderTarget, ComponentOf_RenderTarget())) {
        return;
    }

    auto data = GetCamera2DData(entity);
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

LocalFunction(OnCameraAspectChanged, void, Entity entity, float oldAspect, float newAspect) {
    UpdateProjectionMatrix(entity);
}

LocalFunction(OnCamera2DPixelsPerUnitChanged, void, Entity entity, float before, float after) {
    UpdateProjectionMatrix(entity);
}

LocalFunction(OnCameraViewportChanged, void, Entity entity, v4f before, v4f after) {
    UpdateProjectionMatrix(entity);
}

LocalFunction(OnCameraRenderTargetChanged, void, Entity entity, Entity before, Entity after) {
    UpdateProjectionMatrix(entity);
}

LocalFunction(OnRenderTargetSizeChanged, void, Entity entity, v2i oldValue, v2i newValue) {
    for_entity(camera, data, Camera2D) {
        if(GetCameraRenderTarget(camera) == entity) {
            UpdateProjectionMatrix(camera);
        }
    }
}

DefineService(Camera2D)
        RegisterSubscription(Camera2DPixelsPerUnitChanged, OnCamera2DPixelsPerUnitChanged, 0)
        RegisterSubscription(CameraViewportChanged, OnCameraViewportChanged, 0)
        RegisterSubscription(CameraRenderTargetChanged, OnCameraRenderTargetChanged, 0)
        RegisterSubscription(Camera2DAdded, UpdateProjectionMatrix, 0)
        RegisterSubscription(RenderTargetSizeChanged, OnRenderTargetSizeChanged, 0)
EndService()
