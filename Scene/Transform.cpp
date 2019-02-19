//
// Created by Kim Johannsen on 27/01/2018.
//

#include <cglm/cglm.h>
#include <Foundation/Invalidation.h>
#include <Foundation/AppNode.h>
#include "Transform.h"
#include "Scene.h"

API_EXPORT v3f TransformPoint(Entity sourceSpace, Entity destinationSpace, v3f sourcePoint) {
    v4f sourcePoint4 = {
        sourcePoint.x,
        sourcePoint.y,
        sourcePoint.z,
        1.0f
    };

    auto sourceWorld = GetTransformGlobalMatrix(sourceSpace);

    auto destinationWorld = GetTransformGlobalMatrix(destinationSpace);
    m4x4f destinationWorldInv;
    glm_mat4_inv((vec4*)&destinationWorld, (vec4*)&destinationWorldInv);

    v4f worldSpacePoint = {sourcePoint.x, sourcePoint.y, sourcePoint.z, 0.0f}, destinationPoint;

    // Transform from local into world space
    if(IsEntityValid(sourceSpace)) {
        glm_mat4_mulv((vec4*)&sourceWorld, &sourcePoint4.x, &worldSpacePoint.x);
    }

    // Transform from world back into destination local space
    if(IsEntityValid(destinationSpace)) {
        glm_mat4_mulv((vec4 *) &destinationWorldInv, &worldSpacePoint.x, &destinationPoint.x);
    }

    return {destinationPoint.x, destinationPoint.y, destinationPoint.z};
}

API_EXPORT v3f TransformNormal(Entity sourceSpace, Entity destinationSpace, v3f sourceNormal) {
    v4f sourceNormal4 = {
        sourceNormal.x,
        sourceNormal.y,
        sourceNormal.z,
        0.0f
    };

    auto sourceWorld = GetTransformGlobalMatrix(sourceSpace);
    sourceWorld.w = {0, 0, 0, 1};

    auto destinationWorldInv = GetTransformGlobalMatrix(destinationSpace);
    destinationWorldInv.w = {0, 0, 0, 1};

    glm_mat4_inv((vec4*)&destinationWorldInv, (vec4*)&destinationWorldInv);

    v4f worldSpaceNormal = {sourceNormal.x, sourceNormal.y, sourceNormal.z, 0.0f}, destinationNormal;

    // Transform from local into world space
    if(IsEntityValid(sourceSpace)) {
        glm_mat4_mulv((vec4 *) &sourceWorld, &sourceNormal4.x, &worldSpaceNormal.x);
    }

    // Transform from world back into destination local space
    if(IsEntityValid(destinationSpace)) {
        glm_mat4_mulv((vec4 *) &destinationWorldInv, &worldSpaceNormal.x, &destinationNormal.x);
    }

    return {destinationNormal.x, destinationNormal.y, destinationNormal.z};
}


API_EXPORT void Move3D(Entity transform, v3f direction, bool relativeToRotation) {
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

API_EXPORT void LookAt(Entity transform, v3f origin, v3f direction, v3f up) {
    v4f quat, conj;
    m4x4f m;
    v3f right;

    glm_vec_normalize(&direction.x);

    glm_vec_cross(&direction.x, &up.x, &right.x);
    glm_vec_normalize(&right.x);

    glm_vec_cross(&direction.x, &right.x, &up.x);

    m.x = {right.x, right.y, right.z, 0.0f};
    m.y = {up.x, up.y, up.z, 0.0f};
    m.z = {direction.x, direction.y, direction.z, 0.0f};
    m.w = {origin.x, origin.y, origin.z, 1.0f};

    glm_mat4_quat((vec4*)&m.x, &quat.x);
    glm_quat_conjugate(&quat.x, &conj.x);

    SetRotationQuat3D(transform, conj);
    SetPosition3D(transform, origin);
}

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

    SetTransformLocalMatrix(entity, *(m4x4f *) srMat);
}

static inline void ValidateTransform(Entity entity, Transform *transformData) {
    auto local = transformData->TransformLocalMatrix;
    auto parent = GetOwner(entity);

    m4x4f global = local;

    if(transformData->TransformHierarchyLevel > 0 && HasComponent(parent, ComponentOf_Transform()) > 0) {
        auto parentGlobalMatrix = GetTransformGlobalMatrix(parent);
        glm_mat4_mul((vec4*)&parentGlobalMatrix, (vec4*)&local, (vec4*)&global);

    }

    SetTransformGlobalMatrix(entity, global);
}

static void CalculateHierarchyLevel(Entity entity) {
    if(HasComponent(entity, ComponentOf_Transform())) {
        auto parent = GetOwner(entity);
        s32 level = 0;
        while(HasComponent(parent, ComponentOf_Transform())) {
            level++;
            parent = GetOwner(parent);
        }

        GetTransformData(entity)->TransformHierarchyLevel = level;
        Invalidate(entity);

        for_children(child, Children, entity) {
            CalculateHierarchyLevel(child);
        }
    }
}

LocalFunction(OnLocalChanged, void, Entity entity) {
    UpdateLocalTransform(entity);
}

LocalFunction(OnRotationEuler3DChanged, void, Entity entity) {
    auto data = GetTransformData(entity);

    v4f qx, qy, qz, t;
    glm_quat(&qx.x, glm_rad(data->RotationEuler3D.x), 1.0f, 0.0f, 0.0f);
    glm_quat(&qy.x, glm_rad(data->RotationEuler3D.y), 0.0f, 1.0f, 0.0f);
    glm_quat(&qz.x, glm_rad(data->RotationEuler3D.z), 0.0f, 0.0f, 1.0f);
    glm_quat_mul(&qy.x, &qx.x, &t.x);
    glm_quat_mul(&t.x, &qz.x, &data->RotationQuat3D.x);

    UpdateLocalTransform(entity);
}

LocalFunction(OnRotationQuat3DChanged, void, Entity entity) {
    auto data = GetTransformData(entity);
    auto& q = data->RotationQuat3D;

    v3f euler = {glm_deg(atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * q.x * q.x - 2 * q.z * q.z)),
                 glm_deg(atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * q.y * q.y - 2 * q.z * q.z)),
                 glm_deg(asin(2 * q.x * q.y + 2 * q.z * q.w))};

    data->RotationEuler3D = euler;

    UpdateLocalTransform(entity);
}

LocalFunction(OnTransformValidation, void, Entity component) {
    s32 level = 0;
    bool hasAny = false;
    do
    {
        hasAny = false;

        for_entity(entity, transformData, Transform) {
            if(transformData->TransformHierarchyLevel == level) {
                hasAny = true;
            } else {
                continue;
            }

            ValidateTransform(entity, transformData);
        }

        level++;
    } while(hasAny);
}

LocalFunction(OnAdded, void, Entity component, Entity entity) {
    SetTransformLocalMatrix(entity, m4x4f_Identity);
    SetTransformGlobalMatrix(entity, m4x4f_Identity);
    OnRotationEuler3DChanged(entity);
    SetScale3D(entity, {1.0f, 1.0f, 1.0f});

    CalculateHierarchyLevel(entity);
    Invalidate(entity);
}

LocalFunction(OnOwnerChanged, void, Entity entity, Entity oldOwner, Entity newOwner) {
    CalculateHierarchyLevel(entity);
}

BeginUnit(Transform)
    BeginComponent(Transform)
        RegisterBase(SceneNode)
        RegisterProperty(m4x4f, TransformLocalMatrix)
        RegisterPropertyReadOnly(m4x4f, TransformGlobalMatrix)
        RegisterPropertyReadOnly(s32, TransformHierarchyLevel)
        RegisterProperty(v3f, Position3D)
        RegisterProperty(v3f, Scale3D)
        RegisterProperty(v3f, RotationEuler3D)
        RegisterPropertyReadOnly(v4f, RotationQuat3D)
        RegisterProperty(v2f, Position2D)
        RegisterProperty(v2f, Scale2D)
        RegisterProperty(float, Rotation2D)
        RegisterProperty(float, Distance2D)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Scale3D()), OnLocalChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Position3D()), OnLocalChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RotationQuat3D()), OnRotationQuat3DChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_RotationEuler3D()), OnRotationEuler3DChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Position2D()), OnLocalChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Scale2D()), OnLocalChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Rotation2D()), OnRotationEuler3DChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Distance2D()), OnLocalChanged, 0)

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Owner()), OnOwnerChanged, 0)
    RegisterSubscription(EventOf_EntityComponentAdded(), OnAdded, ComponentOf_Transform())
    RegisterSubscription(EventOf_Validate(), OnTransformValidation, ComponentOf_Transform())
EndUnit()
