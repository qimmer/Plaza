//
// Created by Kim Johannsen on 28/01/2018.
//

#ifndef PLAZA_TRANSFORM2D_H
#define PLAZA_TRANSFORM2D_H

#include <Core/NativeUtils.h>

Unit(Transform2D)
    Component(Transform2D)

        Property(v2f, Position2D)
        Property(float, Rotation2D)
        Property(v2f, Scale2D)
        Property(float, Distance2D)

#endif //PLAZA_TRANSFORM2D_H
