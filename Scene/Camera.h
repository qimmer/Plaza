//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_CAMERA_H
#define PLAZA_CAMERA_H

#include <Core/NativeUtils.h>


Unit(Camera)
    Component(Camera)

            Property(m4x4f, CameraViewMatrix)
            Property(m4x4f, CameraProjectionMatrix)
            Property(m4x4f, CameraInvViewProjectionMatrix)
            Property(rgba8, CameraClearColor)
            Property(v4f, CameraViewport)
            Property(bool, CameraClear)
            Property(u8, CameraLayer)
            Property(Entity, CameraRenderTarget)
            Property(float, CameraNearClip)
            Property(float, CameraFarClip)

v3f GetCameraPickRayPoint(Entity camera, v2f viewportOffset, float d);

#endif //PLAZA_CAMERA_H
