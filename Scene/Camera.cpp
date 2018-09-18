//
// Created by Kim Johannsen on 27/01/2018.
//

#include "Camera.h"
#include "Transform.h"
#include "SceneNode.h"
#include <cglm/cglm.h>

struct Camera {
    rgba8 CameraClearColor;
    v2f CameraViewportOffset, CameraViewportSize;
    bool CameraClear;
    Entity CameraRenderTarget;    
};

BeginUnit(Camera)
    BeginComponent(Camera)
        RegisterBase(Transform)
        RegisterBase(SceneNode)

        RegisterProperty(rgba8, CameraClearColor)
        RegisterProperty(v2f, CameraViewportOffset)
        RegisterProperty(v2f, CameraViewportSize)
        RegisterProperty(bool, CameraClear)
        RegisterProperty(Entity, CameraRenderTarget)
    EndComponent()
EndUnit()
