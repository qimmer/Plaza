//
// Created by Kim Johannsen on 27/01/2018.
//

#include "Camera.h"
#include "Transform.h"
#include "SceneNode.h"
#include <cglm/cglm.h>

struct Camera {
    Camera() : CameraViewport({0.0f, 0.0f, 1.0f, 1.0f}) {}

    m4x4f CameraViewMatrix, CameraProjectionMatrix;
    v4f CameraViewport;
    Entity CameraRenderTarget;
    rgba8 CameraClearColor;
    bool CameraClear;
    float CameraNearClip, CameraFarClip;
    u8 CameraLayer;
};

DefineService(Camera)
EndService()

DefineComponent(Camera)
    Abstract()
    Dependency(Transform)
    Dependency(SceneNode)
    DefineProperty(rgba8, CameraClearColor)
    DefineProperty(bool, CameraClear)
    DefineProperty(float, CameraNearClip)
    DefineProperty(float, CameraFarClip)
    DefineProperty(v4f, CameraViewport)
    DefineProperty(Entity, CameraRenderTarget)
    DefineProperty(u8, CameraLayer)
    DefineProperty(m4x4f, CameraViewMatrix)
    DefineProperty(m4x4f, CameraProjectionMatrix)
EndComponent()

DefineComponentProperty(Camera, m4x4f, CameraViewMatrix)
DefineComponentProperty(Camera, m4x4f, CameraProjectionMatrix)
DefineComponentProperty(Camera, rgba8, CameraClearColor)
DefineComponentProperty(Camera, v4f, CameraViewport)
DefineComponentProperty(Camera, Entity, CameraRenderTarget)
DefineComponentProperty(Camera, bool, CameraClear)
DefineComponentProperty(Camera, float, CameraNearClip)
DefineComponentProperty(Camera, float, CameraFarClip)
DefineComponentProperty(Camera, u8, CameraLayer)

static void UpdateViewMatrix(Entity camera) {
    m4x4f viewMat;
    m4x4f globalMat = GetGlobalTransform(camera);
    glm_mat4_inv((vec4*)&globalMat.x.x, (vec4*)&viewMat.x.x);
    SetCameraViewMatrix(camera, viewMat);
}

static void OnGlobalTransformChanged(Entity entity, m4x4f oldValue, m4x4f newValue) {
    if(HasCamera(entity)) {
        UpdateViewMatrix(entity);
    }
}
static void OnCameraAdded(Entity entity) {
    UpdateViewMatrix(entity);
}

static bool ServiceStart() {
    SubscribeGlobalTransformChanged(OnGlobalTransformChanged);
    SubscribeCameraAdded(OnCameraAdded);
    return true;
}

static bool ServiceStop() {
    UnsubscribeGlobalTransformChanged(OnGlobalTransformChanged);
    UnsubscribeCameraAdded(OnCameraAdded);
    return true;
}