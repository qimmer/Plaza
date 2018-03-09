//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_CAMERA2D_H
#define PLAZA_CAMERA2D_H

#include <Core/Entity.h>


    DeclareComponent(Camera2D)
    DeclareService(Camera2D)

    DeclareComponentPropertyReactive(Camera2D, float, Camera2DPixelsPerUnit)

#endif //PLAZA_CAMERA2D_H
