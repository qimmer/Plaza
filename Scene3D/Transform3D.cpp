//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Transform.h>
#include "Transform3D.h"
#undef __SSE2__
#include <cglm/cglm.h>
#include <Core/Types.h>


struct Transform3D {
    Transform3D() : Scale3D({1.0f, 1.0f, 1.0f}), Position3D({0.0f, 0.0f, 0.0f}), RotationEuler3D({0.0f, 0.0f, 0.0f}), RotationQuat3D({0.0f, 0.0f, 0.0f, 1.0f}) {
    }
    v3f Position3D, Scale3D, RotationEuler3D;
    v4f RotationQuat3D;
};

DefineComponent(Transform3D)
    Dependency(Transform)
    DefineProperty(v3f, Position3D)
    DefineProperty(v3f, Scale3D)
    DefineProperty(v3f, RotationEuler3D)
EndComponent()

DefineService(Transform3D)
EndService()

DefineComponentPropertyReactive(Transform3D, v3f, Position3D)
DefineComponentPropertyReactive(Transform3D, v3f, RotationEuler3D)
DefineComponentPropertyReactive(Transform3D, v4f, RotationQuat3D)
DefineComponentPropertyReactive(Transform3D, v3f, Scale3D)

static void UpdateLocalTransform(Entity entity) {
    auto position = GetPosition3D(entity);
    float pos4[] = {position.x, position.y, position.z, 0.0f};
    auto rotationQuat = GetRotationQuat3D(entity);
    auto scale = GetScale3D(entity);

    mat4 scaleMat, rotationMat, srMat;
    glm_scale_make(scaleMat, &scale.x);
    glm_quat_mat4(&rotationQuat.x, rotationMat);
    glm_mul(scaleMat, rotationMat, srMat);
    glm_vec4_add(srMat[3], pos4, srMat[3]);

    SetLocalTransform(entity, *(m4x4f*)srMat);
}

static void OnPosition3DChanged(Entity entity, v3f before, v3f after) {
    UpdateLocalTransform(entity);
}

static void OnRotationEuler3DChanged(Entity entity, v3f before, v3f after) {
    v4f qx, qy, qz, t;
    glm_quat(&qx.x, glm_rad(after.x), 1.0f, 0.0f, 0.0f);
    glm_quat(&qy.x, glm_rad(after.y), 0.0f, 1.0f, 0.0f);
    glm_quat(&qz.x, glm_rad(after.z), 0.0f, 0.0f, 1.0f);
    glm_quat_mulv(&qy.x, &qx.x, &t.x);
    glm_quat_mulv(&t.x, &qz.x, &GetTransform3D(entity)->RotationQuat3D.x);

    UpdateLocalTransform(entity);
}

static void OnRotationQuat3DChanged(Entity entity, v4f before, v4f q) {
    v3f euler = { glm_deg(atan2(2*q.x*q.w-2*q.y*q.z, 1 - 2*q.x*q.x - 2*q.z*q.z)),
                  glm_deg(atan2(2*q.y*q.w-2*q.x*q.z, 1 - 2*q.y*q.y - 2*q.z*q.z)),
                  glm_deg(asin(2*q.x*q.y + 2*q.z*q.w)) };

    GetTransform3D(entity)->RotationEuler3D = euler;

    UpdateLocalTransform(entity);
}

static bool ServiceStart() {
    SubscribeScale3DChanged(OnPosition3DChanged);
    SubscribePosition3DChanged(OnPosition3DChanged);
    SubscribeRotationQuat3DChanged(OnRotationQuat3DChanged);
    SubscribeRotationEuler3DChanged(OnRotationEuler3DChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeScale3DChanged(OnPosition3DChanged);
    UnsubscribePosition3DChanged(OnPosition3DChanged);
    UnsubscribeRotationQuat3DChanged(OnRotationQuat3DChanged);
    UnsubscribeRotationEuler3DChanged(OnRotationEuler3DChanged);
    return true;
}

