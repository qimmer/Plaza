//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Transform.h>
#include "Transform3D.h"
#include <cglm/cglm.h>


    struct Transform3D {
        Transform3D() : Scale3D({1.0f, 1.0f, 1.0f}) {
        }
        v3f Position3D, Scale3D, RotationEuler3D;
        v4f RotationQuat3D;
    };

    DefineComponent(Transform3D)
        Dependency(Transform)
        DefineProperty(v3f, Position3D)
        DefineProperty(v3f, Scale3D)
        DefineProperty(v3f, RotationEuler3D)
        DefineProperty(v4f, RotationQuat3D)
    EndComponent()

    DefineService(Transform3D)
    EndService()

    DefineComponentProperty(Transform3D, v3f, Position3D)
    DefineComponentProperty(Transform3D, v3f, RotationEuler3D)
    DefineComponentProperty(Transform3D, v4f, RotationQuat3D)
    DefineComponentProperty(Transform3D, v3f, Scale3D)

    static void OnRotationEuler3DChanged(Entity entity, v3f before, v3f after) {
        v4f qx, qy, qz, t;
        glm_quat(&qx, after.x, 1.0f, 0.0f, 0.0f);
        glm_quat(&qy, after.y, 0.0f, 1.0f, 0.0f);
        glm_quat(&qz, after.z, 0.0f, 0.0f, 1.0f);
        glm_quat_mulv(&qx, &qy, &t);
        glm_quat_mulv(&t, &qz, &GetTransform3D(entity)->RotationQuat3D);
    }

    static void OnRotationQuat3DChanged(Entity entity, v4f before, v4f q) {
        v3f euler = { atan2(2*q.x*q.w-2*q.y*q.z, 1 - 2*q.x*q.x - 2*q.z*q.z),
                      atan2(2*q.y*q.w-2*q.x*q.z, 1 - 2*q.y*q.y - 2*q.z*q.z),
                      asin(2*q.x*q.y + 2*q.z*q.w) };
        GetTransform3D(entity)->RotationEuler3D = euler;
    }

    static bool ServiceStart() {
        SubscribeRotationQuat3DChanged(OnRotationQuat3DChanged);
        SubscribeRotationEuler3DChanged(OnRotationEuler3DChanged);
        return true;
    }

    static bool ServiceStop() {
        UnsubscribeRotationQuat3DChanged(OnRotationQuat3DChanged);
        UnsubscribeRotationEuler3DChanged(OnRotationEuler3DChanged);
        return true;
    }
}
