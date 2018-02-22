//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_TRANSFORM3D_H
#define PLAZA_TRANSFORM3D_H

#include <Core/Entity.h>


    DeclareComponent(Transform3D)
    DeclareService(Transform3D)

    DeclareComponentProperty(Transform3D, v3f, Position3D)
    DeclareComponentProperty(Transform3D, v3f, RotationEuler3D)
    DeclareComponentProperty(Transform3D, v4f, RotationQuat3D)
    DeclareComponentProperty(Transform3D, v3f, Scale3D)

#endif //PLAZA_TRANSFORM3D_H
