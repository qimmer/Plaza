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

    BeginUnit(Camera3D)
    BeginComponent(Camera3D)
        RegisterProperty(float, Camera3DFov)
        RegisterProperty(float, Camera3DAspectRatio)
    EndComponent()

    RegisterProperty(float, Camera3DFov)
RegisterProperty(float, Camera3DAspectRatio)

    static void UpdateProjectionMatrix(Entity entity) {
        if(!HasComponent(entity, ComponentOf_Camera3D())) return;

        auto data = GetCamera3DData(entity);

        auto viewport = GetCameraViewport(entity);

        m4x4f projection;
        glm_perspective(glm_rad(data->Camera3DFov), data->Camera3DAspectRatio, GetCameraNearClip(entity), GetCameraFarClip(entity), (vec4*)&projection.x.x);

        vec3 scale = {1.0f, 1.0f, -1.0f};
        glm_scale((vec4*)&projection.x, scale);

        SetCameraProjectionMatrix(entity, projection);
    }

    LocalFunction(OnCamera3DFovChanged, void, Entity entity, float before, float after) {
        UpdateProjectionMatrix(entity);
    }

    LocalFunction(OnCameraViewportChanged, void, Entity entity, v4f before, v4f after) {
        UpdateProjectionMatrix(entity);
    }

    LocalFunction(OnCamera3DAdded, void, Entity entity) {
        UpdateProjectionMatrix(entity);
    }

DefineService(Camera3D)
        RegisterSubscription(Camera3DAdded, OnCamera3DAdded, 0)
        RegisterSubscription(CameraNearClipChanged, OnCamera3DFovChanged, 0)
        RegisterSubscription(CameraFarClipChanged, OnCamera3DFovChanged, 0)
        RegisterSubscription(Camera3DFovChanged, OnCamera3DFovChanged, 0)
        RegisterSubscription(Camera3DAspectRatioChanged, OnCamera3DFovChanged, 0)
        RegisterSubscription(CameraViewportChanged, OnCameraViewportChanged, 0)
EndService()
