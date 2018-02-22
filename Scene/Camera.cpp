//
// Created by Kim Johannsen on 27/01/2018.
//

#include "Camera.h"
#include "Transform.h"
#include "SceneNode.h"


    struct Camera {
        m4x4f CameraViewMatrix, CameraProjectionMatrix;
        v4i CameraViewport;
        Entity CameraRenderTarget;
        rgba8 CameraClearColor;
        bool CameraClear;
        float CameraNearClip, CameraFarClip;
    };

    DefineComponent(Camera)
        Abstract()
        Dependency(Transform)
        Dependency(SceneNode)
        DefineProperty(rgba8, CameraClearColor)
        DefineProperty(bool, CameraClear)
        DefineProperty(v4i, CameraViewport)
        DefineProperty(Entity, CameraRenderTarget)
    EndComponent()

    DefineComponentProperty(Camera, m4x4f, CameraViewMatrix)
    DefineComponentProperty(Camera, m4x4f, CameraProjectionMatrix)
    DefineComponentProperty(Camera, rgba8, CameraClearColor)
    DefineComponentProperty(Camera, v4i, CameraViewport)
    DefineComponentProperty(Camera, Entity, CameraRenderTarget)
    DefineComponentProperty(Camera, bool, CameraClear)
    DefineComponentProperty(Camera, float, CameraNearClip)
    DefineComponentProperty(Camera, float, CameraFarClip)
