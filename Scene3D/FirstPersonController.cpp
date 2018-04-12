//
// Created by Kim Johannsen on 30-03-2018.
//

#include <Core/Entity.h>
#include <Logic/State.h>
#include <Core/cglm/include/cglm/util.h>
#include <Foundation/AppLoop.h>
#include <cglm/cglm.h>
#include "FirstPersonController.h"
#include "Transform3D.h"

struct FirstPersonController {
    FirstPersonController()
        : FirstPersonDamping(10.0f),
          FirstPersonAcceleration(4.0f),
          FirstPersonSpeed(1.0f) {}

    Entity
        FirstPersonActiveState,
        FirstPersonForwardState,
        FirstPersonBackState,
        FirstPersonLeftState,
        FirstPersonRightState,
        FirstPersonUpState,
        FirstPersonDownState,
        FirstPersonLookLeftState,
        FirstPersonLookRightState,
        FirstPersonLookUpState,
        FirstPersonLookDownState;

    float FirstPersonDamping, FirstPersonAcceleration, FirstPersonSpeed;

    v3f FirstPersonVelocity;
};

DefineComponent(FirstPersonController)
    DefinePropertyReactive(float, FirstPersonDamping)
    DefinePropertyReactive(float, FirstPersonAcceleration)
    DefinePropertyReactive(float, FirstPersonSpeed)
    DefinePropertyReactive(Entity, FirstPersonActiveState)
    DefinePropertyReactive(Entity, FirstPersonBackState)
    DefinePropertyReactive(Entity, FirstPersonForwardState)
    DefinePropertyReactive(Entity, FirstPersonLeftState)
    DefinePropertyReactive(Entity, FirstPersonRightState)
    DefinePropertyReactive(Entity, FirstPersonUpState)
    DefinePropertyReactive(Entity, FirstPersonDownState)
    DefinePropertyReactive(Entity, FirstPersonLookLeftState)
    DefinePropertyReactive(Entity, FirstPersonLookRightState)
    DefinePropertyReactive(Entity, FirstPersonLookUpState)
    DefinePropertyReactive(Entity, FirstPersonLookDownState)
    DefinePropertyReactive(v3f, FirstPersonVelocity)
EndComponent()

DefineComponentPropertyReactive(FirstPersonController, float, FirstPersonDamping)
DefineComponentPropertyReactive(FirstPersonController, float, FirstPersonAcceleration)
DefineComponentPropertyReactive(FirstPersonController, float, FirstPersonSpeed)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonActiveState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonForwardState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonBackState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonLeftState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonRightState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonUpState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonDownState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonLookLeftState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonLookRightState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonLookUpState)
DefineComponentPropertyReactive(FirstPersonController, Entity, FirstPersonLookDownState)
DefineComponentPropertyReactive(FirstPersonController, v3f, FirstPersonVelocity)

static void OnUpdate(double deltaTime) {
    for_entity(camera, FirstPersonController) {
        auto data = GetFirstPersonController(camera);

        auto position = GetPosition3D(camera);
        auto euler = GetRotationEuler3D(camera);

        // Exponentiation base for velocity damping
        float D2 = pow(1.f - data->FirstPersonDamping / 60.f, 60.f);

        // Damp velocity (framerate-independent)
        auto dampDelta = pow(D2, deltaTime);
        data->FirstPersonVelocity.x *= dampDelta;
        data->FirstPersonVelocity.y *= dampDelta;
        data->FirstPersonVelocity.z *= dampDelta;

        if(!IsEntityValid(data->FirstPersonActiveState) ||
           GetStateValue(data->FirstPersonActiveState) > 0.5f) {

            if(IsEntityValid(data->FirstPersonLookDownState) &&
               IsEntityValid(data->FirstPersonLookLeftState) &&
               IsEntityValid(data->FirstPersonLookUpState) &&
               IsEntityValid(data->FirstPersonLookRightState)) {
                v3f rotateDelta = {
                    GetStateValue(data->FirstPersonLookDownState) - GetStateValue(data->FirstPersonLookUpState),
                    -GetStateValue(data->FirstPersonLookLeftState) + GetStateValue(data->FirstPersonLookRightState),
                    0.0f
                };

                auto oldEuler = euler;

                euler.x += rotateDelta.x;
                euler.y += rotateDelta.y;
                euler.z = 0.0f;

                euler.x = glm_max(glm_min(euler.x, 89.0f), -89.0f);

                if(glm_vec_distance(&oldEuler.x, &euler.x) > FLT_EPSILON) {
                    SetRotationEuler3D(camera, euler);
                }
            }

            if(IsEntityValid(data->FirstPersonDownState) &&
               IsEntityValid(data->FirstPersonLeftState) &&
               IsEntityValid(data->FirstPersonUpState) &&
               IsEntityValid(data->FirstPersonRightState) &&
               IsEntityValid(data->FirstPersonForwardState) &&
               IsEntityValid(data->FirstPersonBackState)) {
                v3f moveDelta = {
                    -GetStateValue(data->FirstPersonLeftState) + GetStateValue(data->FirstPersonRightState),
                    -GetStateValue(data->FirstPersonDownState) + GetStateValue(data->FirstPersonUpState),
                    -GetStateValue(data->FirstPersonBackState) + GetStateValue(data->FirstPersonForwardState),
                };

                data->FirstPersonVelocity.x += data->FirstPersonAcceleration * moveDelta.x * deltaTime;
                data->FirstPersonVelocity.y += data->FirstPersonAcceleration * moveDelta.y * deltaTime;
                data->FirstPersonVelocity.z += data->FirstPersonAcceleration * moveDelta.z * deltaTime;

            }
        }

        data->FirstPersonVelocity.x = glm_max(glm_min(data->FirstPersonVelocity.x, 1.0f), -1.0f);
        data->FirstPersonVelocity.y = glm_max(glm_min(data->FirstPersonVelocity.y, 1.0f), -1.0f);
        data->FirstPersonVelocity.z = glm_max(glm_min(data->FirstPersonVelocity.z, 1.0f), -1.0f);

        v3f moveDelta = data->FirstPersonVelocity;
        moveDelta.x *= deltaTime * data->FirstPersonSpeed;
        moveDelta.y *= deltaTime * data->FirstPersonSpeed;
        moveDelta.z *= deltaTime * data->FirstPersonSpeed;

        Move3D(camera, moveDelta, true);
    }
}

DefineService(FirstPersonController)
    Subscribe(AppUpdate, OnUpdate)
EndService()