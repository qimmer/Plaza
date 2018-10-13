//
// Created by Kim Johannsen on 27/01/2018.
//

#include "Camera.h"
#include "Transform.h"
#include "Frustum.h"

struct Camera {
};

BeginUnit(Camera)
    BeginComponent(Camera)
        RegisterBase(Transform)
        RegisterBase(Frustum)
    EndComponent()
EndUnit()
