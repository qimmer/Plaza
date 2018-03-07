//
// Created by Kim Johannsen on 28/01/2018.
//

#include "Camera3D.h"
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
        if(!HasCamera3D(entity)) return;

        auto data = GetCamera3D(entity);

        auto viewport = GetCameraViewport(entity);

        m4x4f projection;
        glm_perspective(glm_rad(data->Camera3DFov), viewport.x / viewport.y, GetCameraNearClip(entity), GetCameraFarClip(entity), (vec4*)&projection);

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
        SubscribeCamera3DFovChanged(OnCamera3DFovChanged);
        SubscribeCameraViewportChanged(OnCameraViewportChanged);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeCamera3DAdded(OnCamera3DAdded);
        UnsubscribeCamera3DFovChanged(OnCamera3DFovChanged);
        UnsubscribeCameraViewportChanged(OnCameraViewportChanged);
        return true;
    }

