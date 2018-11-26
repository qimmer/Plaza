//
// Created by Kim on 21-11-2018.
//

#ifndef PLAZA_VELOCITY_H
#define PLAZA_VELOCITY_H

#include <Core/NativeUtils.h>

struct Movement {
    v3f MovementVelocity, MovementTorque;
    Entity MovementTransform;
};

struct Accelerator {
    Entity AcceleratorTarget;
    v3f AcceleratorAcceleration;
};

Unit(Velocity)
    Component(Movement)
        Property(v3f, MovementVelocity)
        Property(v3f, MovementTorque)
        ReferenceProperty(Transform, MovementTransform)

    Component(Accelerator)
        ReferenceProperty(Movement, AcceleratorTarget)
        Property(v3f, AcceleratorAcceleration)

    Declare(AppLoop, Velocity)
    Declare(StopWatch, Velocity)

#endif //PLAZA_VELOCITY_H
