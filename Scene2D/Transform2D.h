//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_TRANSFORM2D_H
#define PLAZA_TRANSFORM2D_H

#include <Core/Entity.h>


    DeclareComponent(Transform2D)
    DeclareService(Transform2D)

    DeclareComponentProperty(Transform2D, v2f, Position2D)
    DeclareComponentProperty(Transform2D, float, Rotation2D)
    DeclareComponentProperty(Transform2D, v2f, Scale2D)

#endif //PLAZA_TRANSFORM2D_H
