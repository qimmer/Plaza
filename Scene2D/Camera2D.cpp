//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Camera2D.h"
#include <Scene/Camera.h>
#include <cglm/cglm.h>


    struct Camera2D {
        float Camera2DPixelsPerUnit;
    };

    DefineComponent(Camera2D)
    EndComponent()

    DefineService(Camera2D)
    EndService()

    DefineComponentProperty(Camera2D, float, Camera2DPixelsPerUnit)

    static void UpdateProjectionMatrix(Entity entity) {
        auto data = GetCamera2D(entity);

        v4i viewport = GetCameraViewport(entity);
        v2f viewportExtent = {float(viewport.z - viewport.x) * 0.5f, float(viewport.w - viewport.y) * 0.5f};

        viewportExtent.x *= data->Camera2DPixelsPerUnit;
        viewportExtent.y *= data->Camera2DPixelsPerUnit;

        m4x4f projection;
        glm_ortho(-viewportExtent.x, viewportExtent.x, -viewportExtent.y, viewportExtent.y, GetCameraNearClip(entity), GetCameraFarClip(entity), (vec4*)&projection);

        SetCameraProjectionMatrix(entity, projection);
    }

    static void OnCamera2DPixelsPerUnitChanged(Entity entity, float before, float after) {
        UpdateProjectionMatrix(entity);
    }

    static void OnCameraViewportChanged(Entity entity, v4i before, v4i after) {
        UpdateProjectionMatrix(entity);
    }

    static void OnCameraRenderTargetChanged(Entity entity, Entity before, Entity after) {
        UpdateProjectionMatrix(entity);
    }

    static bool ServiceStart() {
        SubscribeCamera2DPixelsPerUnitChanged(OnCamera2DPixelsPerUnitChanged);
        SubscribeCameraViewportChanged(OnCameraViewportChanged);
        SubscribeCameraRenderTargetChanged(OnCameraRenderTargetChanged);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeCamera2DPixelsPerUnitChanged(OnCamera2DPixelsPerUnitChanged);
        UnsubscribeCameraViewportChanged(OnCameraViewportChanged);
        UnsubscribeCameraRenderTargetChanged(OnCameraRenderTargetChanged);
        return true;
    }

