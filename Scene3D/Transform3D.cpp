//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Transform.h>
#include "Transform3D.h"

#undef __SSE2__

#include <cglm/cglm.h>
#include <Core/Types.h>

struct Transform3D {
    Transform3D() : Scale3D({1.0f, 1.0f, 1.0f}), Position3D({0.0f, 0.0f, 0.0f}), RotationEuler3D({0.0f, 0.0f, 0.0f}),
                    RotationQuat3D({0.0f, 0.0f, 0.0f, 1.0f}) {
    }

    v3f Position3D, Scale3D, RotationEuler3D;
    v4f RotationQuat3D;
};

BeginUnit(Transform3D)
    BeginComponent(Transform3D)
    RegisterBase(Transform)
    RegisterProperty(v3f, Position3D)
    RegisterProperty(v3f, Scale3D)
    RegisterProperty(v3f, RotationEuler3D)
EndComponent()

RegisterProperty(v3f, Position3D)

RegisterProperty(v3f, RotationEuler3D)

RegisterProperty(v4f, RotationQuat3D)

RegisterProperty(v3f, Scale3D)

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

    SetLocalTransform(entity, *(m4x4f *) srMat);
}

LocalFunction(OnPosition3DChanged, void, Entity entity, v3f before, v3f after) {
    if(memcmp(&before, &after, sizeof(v3f)) != 0) {
        UpdateLocalTransform(entity);
    }
}

LocalFunction(OnRotationEuler3DChanged, void, Entity entity, v3f before, v3f after) {
    v4f qx, qy, qz, t;
    glm_quat(&qx.x, glm_rad(after.x), 1.0f, 0.0f, 0.0f);
    glm_quat(&qy.x, glm_rad(after.y), 0.0f, 1.0f, 0.0f);
    glm_quat(&qz.x, glm_rad(after.z), 0.0f, 0.0f, 1.0f);
    glm_quat_mulv(&qy.x, &qx.x, &t.x);
    glm_quat_mulv(&t.x, &qz.x, &GetTransform3DData(entity)->RotationQuat3D.x);

    UpdateLocalTransform(entity);
}

LocalFunction(OnRotationQuat3DChanged, void, Entity entity, v4f before, v4f q) {
    v3f euler = {glm_deg(atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * q.x * q.x - 2 * q.z * q.z)),
                 glm_deg(atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * q.y * q.y - 2 * q.z * q.z)),
                 glm_deg(asin(2 * q.x * q.y + 2 * q.z * q.w))};

    GetTransform3DData(entity)->RotationEuler3D = euler;

    UpdateLocalTransform(entity);
}

DefineService(Transform3D)
    RegisterSubscription(Scale3DChanged, OnPosition3DChanged, 0)
    RegisterSubscription(Position3DChanged, OnPosition3DChanged, 0)
    RegisterSubscription(RotationQuat3DChanged, OnRotationQuat3DChanged, 0)
    RegisterSubscription(RotationEuler3DChanged, OnRotationEuler3DChanged, 0)
EndService()

void Move3D(Entity transform, v3f direction, bool relativeToRotation) {
    auto position = GetPosition3D(transform);

    if(relativeToRotation) {
        auto euler = GetRotationEuler3D(transform);

        float xUp[] = {1.0f, 0.0f, 0.0f};
        float yUp[] = {0.0f, 1.0f, 0.0f};
        float zUp[] = {0.0f, 0.0f, 1.0f};

        glm_vec_rotate(&direction.x, glm_rad(euler.x), xUp);
        glm_vec_rotate(&direction.x, glm_rad(euler.y), yUp);
        glm_vec_rotate(&direction.x, glm_rad(euler.z), zUp);
    }

    position.x += direction.x;
    position.y += direction.y;
    position.z += direction.z;

    SetPosition3D(transform, position);
}
