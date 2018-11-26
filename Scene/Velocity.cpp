//
// Created by Kim on 21-11-2018.
//

#include "Velocity.h"
#include "Transform.h"
#include <Foundation/AppLoop.h>
#include <Foundation/StopWatch.h>

LocalFunction(OnUpdateMovement, void, Entity appLoop, u64 oldFrame, u64 newFrame) {
    auto deltaTime = GetStopWatchElapsedSeconds(StopWatchOf_Velocity());
    SetStopWatchElapsedSeconds(StopWatchOf_Velocity(), 0.0);

    for_entity(accelerator, data, Accelerator, {
        auto movement = data->AcceleratorTarget;

        if(HasComponent(movement, ComponentOf_Movement())) {
            auto velocity = GetMovementVelocity(movement);
            velocity.x += data->AcceleratorAcceleration.x * deltaTime;
            velocity.y += data->AcceleratorAcceleration.y * deltaTime;
            velocity.z += data->AcceleratorAcceleration.z * deltaTime;
            SetMovementVelocity(movement, velocity);
        }
    });

    for_entity(movement, data, Movement, {
        auto transform = data->MovementTransform;

        if(HasComponent(transform, ComponentOf_Transform())) {
            auto position = GetPosition3D(transform);
            position.x += data->MovementVelocity.x * deltaTime;
            position.y += data->MovementVelocity.y * deltaTime;
            position.z += data->MovementVelocity.z * deltaTime;
            SetPosition3D(transform, position);

            auto euler = GetRotationEuler3D(transform);
            euler.x += data->MovementTorque.x * deltaTime;
            euler.y += data->MovementTorque.y * deltaTime;
            euler.z += data->MovementTorque.z * deltaTime;
            SetRotationEuler3D(transform, euler);
        }
    });
}

BeginUnit(Velocity)
    BeginComponent(Movement)
        RegisterProperty(v3f, MovementVelocity)
        RegisterProperty(v3f, MovementTorque)
        RegisterReferenceProperty(Transform, MovementTransform)
    EndComponent()

    BeginComponent(Accelerator)
        RegisterReferenceProperty(Movement, AcceleratorTarget)
        RegisterProperty(v3f, AcceleratorAcceleration)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnUpdateMovement, AppLoopOf_Velocity())

    SetAppLoopOrder(AppLoopOf_Velocity(), AppLoopOrder_Update * 1.25f);
    SetStopWatchRunning(StopWatchOf_Velocity(), true);
EndUnit()