//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Camera3D.h"
#include <Scene/Camera.h>
#include <cglm/cglm.h>


    struct Camera3D {
        Camera3D() : Camera3DFov(90.0f), Camera3DAspectRatio(1.0f) {}

        float Camera3DFov, Camera3DAspectRatio;
    };

    DefineComponent(Camera3D)
        DefineProperty(float, Camera3DFov)
        DefineProperty(float, Camera3DAspectRatio)
    EndComponent()

    DefineService(Camera3D)
    EndService()

    DefineComponentPropertyReactive(Camera3D, float, Camera3DFov)
DefineComponentPropertyReactive(Camera3D, float, Camera3DAspectRatio)

    static void UpdateProjectionMatrix(Entity entity) {
        if(!HasCamera3D(entity)) return;

        auto data = GetCamera3D(entity);

        auto viewport = GetCameraViewport(entity);

        m4x4f projection;
        glm_perspective(glm_rad(data->Camera3DFov), data->Camera3DAspectRatio, GetCameraNearClip(entity), GetCameraFarClip(entity), (vec4*)&projection.x.x);

        vec3 scale = {1.0f, 1.0f, -1.0f};
        glm_scale((vec4*)&projection.x, scale);

        SetCameraProjectionMatrix(entity, projection);
    }

    static void OnCamera3DFovChanged(Entity entity, float before, float after) {
        UpdateProjectionMatrix(entity);
    }

    static void OnCameraViewportChanged(Entity entity, v4f before, v4f after) {
        UpdateProjectionMatrix(entity);
    }

    static void OnCamera3DAdded(Entity entity) {
        UpdateProjectionMatrix(entity);
    }

    static bool ServiceStart() {
        SubscribeCamera3DAdded(OnCamera3DAdded);
        SubscribeCameraNearClipChanged(OnCamera3DFovChanged);
        SubscribeCameraFarClipChanged(OnCamera3DFovChanged);
        SubscribeCamera3DFovChanged(OnCamera3DFovChanged);
        SubscribeCamera3DAspectRatioChanged(OnCamera3DFovChanged);
        SubscribeCameraViewportChanged(OnCameraViewportChanged);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeCamera3DAdded(OnCamera3DAdded);
        UnsubscribeCameraNearClipChanged(OnCamera3DFovChanged);
        UnsubscribeCameraFarClipChanged(OnCamera3DFovChanged);
        UnsubscribeCamera3DFovChanged(OnCamera3DFovChanged);
        UnsubscribeCamera3DAspectRatioChanged(OnCamera3DFovChanged);
        UnsubscribeCameraViewportChanged(OnCameraViewportChanged);
        return true;
    }

