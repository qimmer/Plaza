//
// Created by Kim on 21-11-2018.
//

#include "Velocity.h"
#include "Transform.h"
#include <Foundation/AppLoop.h>
#include <Foundation/StopWatch.h>

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    Accelerator acceleratorData;
    Movement movementData;

    for_entity_data(accelerator, ComponentOf_Accelerator(), &acceleratorData) {
        auto movement = acceleratorData.AcceleratorTarget;
        movementData = GetMovement(movement);

        movementData.MovementVelocity.x += acceleratorData.AcceleratorAcceleration.x * newData.AppLoopDeltaTime;
        movementData.MovementVelocity.y += acceleratorData.AcceleratorAcceleration.y * newData.AppLoopDeltaTime;
        movementData.MovementVelocity.z += acceleratorData.AcceleratorAcceleration.z * newData.AppLoopDeltaTime;

        SetMovement(movement, movementData);
    }


    for_entity_data(movement, ComponentOf_Movement(), &movementData) {
        auto transform = movementData.MovementTransform;
        auto transformData = GetTransform(transform);

        transformData.Position3D.x += movementData.MovementVelocity.x * newData.AppLoopDeltaTime;
        transformData.Position3D.y += movementData.MovementVelocity.y * newData.AppLoopDeltaTime;
        transformData.Position3D.z += movementData.MovementVelocity.z * newData.AppLoopDeltaTime;

        transformData.RotationEuler3D.x += movementData.MovementTorque.x * newData.AppLoopDeltaTime;
        transformData.RotationEuler3D.y += movementData.MovementTorque.y * newData.AppLoopDeltaTime;
        transformData.RotationEuler3D.z += movementData.MovementTorque.z * newData.AppLoopDeltaTime;

        SetTransform(transform, transformData);
    }
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

    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_Update * 1.25f)
EndUnit()