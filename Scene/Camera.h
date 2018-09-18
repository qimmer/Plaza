//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_CAMERA_H
#define PLAZA_CAMERA_H

#include <Core/NativeUtils.h>

Unit(Camera)
    Component(Camera)
        Property(rgba8, CameraClearColor)
        Property(v2f, CameraViewportOffset)
        Property(v2f, CameraViewportSize)
        Property(bool, CameraClear)
        Property(Entity, CameraRenderTarget)

#endif //PLAZA_CAMERA_H
