//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_CAMERA_H
#define PLAZA_CAMERA_H

#include <Core/Entity.h>


DeclareComponent(Camera)
DeclareService(Camera)

    DeclareComponentPropertyReactive(Camera, m4x4f, CameraViewMatrix)
    DeclareComponentPropertyReactive(Camera, m4x4f, CameraProjectionMatrix)
    DeclareComponentPropertyReactive(Camera, rgba8, CameraClearColor)
    DeclareComponentPropertyReactive(Camera, v4f, CameraViewport)
    DeclareComponentPropertyReactive(Camera, bool, CameraClear)
    DeclareComponentPropertyReactive(Camera, u8, CameraLayer)
    DeclareComponentPropertyReactive(Camera, Entity, CameraRenderTarget)
    DeclareComponentPropertyReactive(Camera, float, CameraNearClip)
    DeclareComponentPropertyReactive(Camera, float, CameraFarClip)

#endif //PLAZA_CAMERA_H
