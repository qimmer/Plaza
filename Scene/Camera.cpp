//
// Created by Kim Johannsen on 27/01/2018.
//

#include "Camera.h"
#include "Transform.h"
#include "SceneNode.h"
#include <cglm/cglm.h>

struct Camera {
    Camera() : CameraViewport({0.0f, 0.0f, 1.0f, 1.0f}) {}

    m4x4f CameraViewMatrix, CameraProjectionMatrix, CameraInvViewProjectionMatrix;
    v4f CameraViewport;
    Entity CameraRenderTarget;
    rgba8 CameraClearColor;
    bool CameraClear;
    float CameraNearClip, CameraFarClip;
    u8 CameraLayer;
};

DefineComponent(Camera)
    Abstract()
    Dependency(Transform)
    Dependency(SceneNode)
    DefinePropertyReactive(rgba8, CameraClearColor)
    DefinePropertyReactive(bool, CameraClear)
    DefinePropertyReactive(float, CameraNearClip)
    DefinePropertyReactive(float, CameraFarClip)
    DefinePropertyReactive(v4f, CameraViewport)
    DefinePropertyReactive(Entity, CameraRenderTarget)
    DefinePropertyReactive(u8, CameraLayer)
    DefinePropertyReactive(m4x4f, CameraViewMatrix)
    DefinePropertyReactive(m4x4f, CameraProjectionMatrix)
EndComponent()

DefineComponentPropertyReactive(Camera, m4x4f, CameraViewMatrix)
DefineComponentPropertyReactive(Camera, m4x4f, CameraProjectionMatrix)
DefineComponentPropertyReactive(Camera, m4x4f, CameraInvViewProjectionMatrix)
DefineComponentPropertyReactive(Camera, rgba8, CameraClearColor)
DefineComponentPropertyReactive(Camera, v4f, CameraViewport)
DefineComponentPropertyReactive(Camera, Entity, CameraRenderTarget)
DefineComponentPropertyReactive(Camera, bool, CameraClear)
DefineComponentPropertyReactive(Camera, float, CameraNearClip)
DefineComponentPropertyReactive(Camera, float, CameraFarClip)
DefineComponentPropertyReactive(Camera, u8, CameraLayer)

v3f GetCameraPickRayPoint(Entity camera, v2f viewportOffset, float d) {
    auto data = GetCamera(camera);

    float mouseXNDC = viewportOffset.x * 2.0f - 1.0f;
    float mouseYNDC = (1.0f - viewportOffset.y) * 2.0f - 1.0f;
    float mousePosNDC[4] = { mouseXNDC, mouseYNDC, d, 1.0f };

    float point[4];
    glm_mat4_mulv((vec4*)&data->CameraInvViewProjectionMatrix, mousePosNDC, point);

    return {point[0] / point[3], point[1] / point[3], point[2] / point[3]};
}

static void UpdateViewMatrix(Entity camera) {
    auto data = GetCamera(camera);

    m4x4f viewMat;
    m4x4f globalMat = GetGlobalTransform(camera);
    glm_mat4_inv((vec4*)&globalMat.x.x, (vec4*)&viewMat.x.x);
    SetCameraViewMatrix(camera, viewMat);
}

static void UpdateInvProjectionMatrix(Entity camera, m4x4f oldValue, m4x4f newValue) {
    auto data = GetCamera(camera);
    m4x4f viewProjMat, invViewProjMat;

    glm_mat4_mul((vec4*)&data->CameraProjectionMatrix, (vec4*)&data->CameraViewMatrix, (vec4*)&viewProjMat);
    glm_mat4_inv((vec4*)&viewProjMat, (vec4*)&invViewProjMat);

    SetCameraInvViewProjectionMatrix(camera, invViewProjMat);
}

static void OnGlobalTransformChanged(Entity entity, m4x4f oldValue, m4x4f newValue) {
    if(HasCamera(entity)) {
        UpdateViewMatrix(entity);
    }
}
static void OnCameraAdded(Entity entity) {
    UpdateViewMatrix(entity);
}

DefineService(Camera)
    Subscribe(GlobalTransformChanged, OnGlobalTransformChanged)
    Subscribe(CameraAdded, OnCameraAdded)
    Subscribe(CameraProjectionMatrixChanged, UpdateInvProjectionMatrix)
    Subscribe(CameraViewMatrixChanged, UpdateInvProjectionMatrix)
EndService()
