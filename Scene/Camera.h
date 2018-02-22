//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_CAMERA_H
#define PLAZA_CAMERA_H

#include <Core/Entity.h>


    DeclareComponent(Camera)

    DeclareComponentProperty(Camera, m4x4f, CameraViewMatrix)
    DeclareComponentProperty(Camera, m4x4f, CameraProjectionMatrix)
    DeclareComponentProperty(Camera, rgba8, CameraClearColor)
    DeclareComponentProperty(Camera, v4i, CameraViewport)
    DeclareComponentProperty(Camera, bool, CameraClear)
    DeclareComponentProperty(Camera, Entity, CameraRenderTarget)
    DeclareComponentProperty(Camera, float, CameraNearClip)
    DeclareComponentProperty(Camera, float, CameraFarClip)

#endif //PLAZA_CAMERA_H
