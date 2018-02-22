//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Camera2D.h"
#include <Scene/Camera.h>
#include <cglm/cglm.h>


    struct Camera3D {
        float Camera3DFov;
    };

    DefineComponent(Camera3D)
    EndComponent()

    DefineService(Camera3D)
    EndService()

    DefineComponentProperty(Camera3D, float, Camera3DFov)

    static void UpdateProjectionMatrix(Entity entity) {
        auto data = GetCamera3D(entity);

        v4i viewport = GetCameraViewport(entity);

        m4x4f projection;
        glm_perspective(data->Camera3DFov, (float)viewport.x / viewport.y, GetCameraNearClip(entity), GetCameraFarClip(entity), (vec4*)&projection);

        SetCameraProjectionMatrix(entity, projection);
    }

    static void OnCamera3DFovChanged(Entity entity, float before, float after) {
        UpdateProjectionMatrix(entity);
    }

    static void OnCameraViewportChanged(Entity entity, v4i before, v4i after) {
        UpdateProjectionMatrix(entity);
    }

    static void OnCameraRenderTargetChanged(Entity entity, Entity before, Entity after) {
        UpdateProjectionMatrix(entity);
    }

    static bool ServiceStart() {
        SubscribeCamera3DFovChangedChanged(OnCamera2DPixelsPerUnitChanged);
        SubscribeCameraViewportChanged(OnCameraViewportChanged);
        SubscribeCameraRenderTargetChanged(OnCameraRenderTargetChanged);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeCamera3DFovChanged(OnCamera2DPixelsPerUnitChanged);
        UnsubscribeCameraViewportChanged(OnCameraViewportChanged);
        UnsubscribeCameraRenderTargetChanged(OnCameraRenderTargetChanged);
        return true;
    }
}
