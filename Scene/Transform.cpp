//
// Created by Kim Johannsen on 27/01/2018.
//

#include <cglm/cglm.h>
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
	auto sourceData = GetWorldTransform(sourceSpace);
    auto sourceWorld = &sourceData.WorldTransformMatrix[0];

	auto destinationData = GetWorldTransform(destinationSpace);
	auto destinationWorld = &destinationData.WorldTransformMatrix[0];

    v4f destinationWorldInv[4];
    glm_mat4_inv((vec4*)destinationWorld, (vec4*)&destinationWorldInv[0].x);

    v4f worldSpacePoint = {sourcePoint.x, sourcePoint.y, sourcePoint.z, 0.0f}, destinationPoint;

    // Transform from local into world space
    if(IsEntityValid(sourceSpace)) {
        glm_mat4_mulv((vec4*)sourceWorld, &sourcePoint4.x, &worldSpacePoint.x);
    }

    // Transform from world back into destination local space
    if(IsEntityValid(destinationSpace)) {
        glm_mat4_mulv((vec4 *) &destinationWorldInv[0].x, &worldSpacePoint.x, &destinationPoint.x);
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

	auto sourceData = GetWorldTransform(sourceSpace);
	auto sourceWorld = &sourceData.WorldTransformMatrix[0];

	v4f sourceWorldNormal[] = {
		sourceWorld[0],
		sourceWorld[1],
		sourceWorld[2],
		{0, 0, 0, 1}
	};

	auto destinationData = GetWorldTransform(destinationSpace);
	auto destinationWorld = &destinationData.WorldTransformMatrix[0];

	v4f destinationWorldInv[4];
	glm_mat4_inv((vec4*)destinationWorld, (vec4*)&destinationWorldInv[0].x);

	v4f worldSpaceNormal = { sourceNormal.x, sourceNormal.y, sourceNormal.z, 0.0f }, destinationNormal;

	// Transform from local into world space
	if (IsEntityValid(sourceSpace)) {
		glm_mat4_mulv((vec4*)&sourceWorldNormal[0].x, &sourceNormal4.x, &worldSpaceNormal.x);
	}

	// Transform from world back into destination local space
	if (IsEntityValid(destinationSpace)) {
		glm_mat4_mulv((vec4 *)&destinationWorldInv[0].x, &worldSpaceNormal.x, &destinationNormal.x);
	}

	return { destinationNormal.x, destinationNormal.y, destinationNormal.z };
}


API_EXPORT void Move3D(Entity transform, v3f direction, bool relativeToRotation) {
    auto transformData = GetTransform(transform);

    auto position = transformData.Position3D;

    if(relativeToRotation) {
        auto euler = transformData.RotationEuler3D;

        float xUp[] = {1.0f, 0.0f, 0.0f};
        float yUp[] = {0.0f, 1.0f, 0.0f};
        float zUp[] = {0.0f, 0.0f, 1.0f};

        glm_vec_rotate(&direction.x, glm_rad(euler.x), xUp);
        glm_vec_rotate(&direction.x, glm_rad(euler.y), yUp);
        glm_vec_rotate(&direction.x, glm_rad(euler.z), zUp);
    }

    transformData.Position3D.x += direction.x;
    transformData.Position3D.y += direction.y;
    transformData.Position3D.z += direction.z;

    SetTransform(transform, transformData);
}

API_EXPORT void LookAt(Entity transform, v3f origin, v3f direction, v3f up) {
    v4f quat, conj;
    v4f m[4];
    v3f right;

    glm_vec_normalize(&direction.x);

    glm_vec_cross(&direction.x, &up.x, &right.x);
    glm_vec_normalize(&right.x);

    glm_vec_cross(&direction.x, &right.x, &up.x);

    m[0] = {right.x, right.y, right.z, 0.0f};
    m[1] = {up.x, up.y, up.z, 0.0f};
    m[2] = {direction.x, direction.y, direction.z, 0.0f};
    m[3] = {origin.x, origin.y, origin.z, 1.0f};

    glm_mat4_quat((vec4*)&m[0].x, &quat.x);
    glm_quat_conjugate(&quat.x, &conj.x);

    auto transformData = GetTransform(transform);
    transformData.RotationQuat3D = conj;
    transformData.Position3D = origin;
    SetTransform(transform, transformData);
}

static void GetLocalTransform(const Transform& data, v4f *localMat) {
    auto position = data.Position3D;
    float pos4[] = {position.x, position.y, position.z, 0.0f};
    auto rotationQuat = data.RotationQuat3D;
    auto scale = data.Scale3D;

    mat4 scaleMat, rotationMat;
    glm_scale_make(scaleMat, &scale.x);
    glm_quat_mat4(&rotationQuat.x, rotationMat);
    glm_mul(scaleMat, rotationMat, (vec4*)localMat);

    glm_vec4_add(&localMat->x, pos4, &localMat->x);
}

static void OnTransformChanged(Entity entity, const Transform& oldData, const Transform& newData) {
    auto validatedData = newData;

    if(memcmp(&oldData.RotationEuler3D, &newData.RotationEuler3D, sizeof(v3f)) != 0) {
        v4f qx, qy, qz, t;
        glm_quat(&qx.x, glm_rad(newData.RotationEuler3D.x), 1.0f, 0.0f, 0.0f);
        glm_quat(&qy.x, glm_rad(newData.RotationEuler3D.y), 0.0f, 1.0f, 0.0f);
        glm_quat(&qz.x, glm_rad(newData.RotationEuler3D.z), 0.0f, 0.0f, 1.0f);
        glm_quat_mul(&qy.x, &qx.x, &t.x);
        glm_quat_mul(&t.x, &qz.x, &validatedData.RotationQuat3D.x);

        SetTransform(entity, validatedData);
        return;
    }

    if(memcmp(&oldData.RotationQuat3D, &newData.RotationQuat3D, sizeof(v4f)) != 0) {
        auto& q = newData.RotationQuat3D;

        v3f euler = {glm_deg(atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * q.x * q.x - 2 * q.z * q.z)),
                     glm_deg(atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * q.y * q.y - 2 * q.z * q.z)),
                     glm_deg(asin(2 * q.x * q.y + 2 * q.z * q.w))};

        validatedData.RotationEuler3D = euler;

        SetTransform(entity, validatedData);
        return;
    }

    v4f localTransform;
    GetLocalTransform(newData, &localTransform);

    auto ownershipData = GetOwnership(entity);
    auto parentWorldTransform = GetWorldTransform(ownershipData.Owner);
    WorldTransform worldTransform;

    glm_mat4_mul((vec4*)&parentWorldTransform.WorldTransformMatrix[0].x, (vec4*)&localTransform.x, (vec4*)&worldTransform.WorldTransformMatrix[0].x);

    SetWorldTransform(entity, parentWorldTransform);
}

static void OnOwnerChanged(Entity entity, const Ownership& oldData, const Ownership& newData) {
    auto ownershipData = GetOwnership(entity);
    auto transformData = GetTransform(entity);
    auto parentWorldTransform = GetWorldTransform(ownershipData.Owner);

    v4f localTransform;
    GetLocalTransform(transformData, &localTransform);

    WorldTransform worldTransform;

    glm_mat4_mul((vec4*)&parentWorldTransform.WorldTransformMatrix[0].x, (vec4*)&localTransform.x, (vec4*)&worldTransform.WorldTransformMatrix[0].x);

    SetWorldTransform(entity, parentWorldTransform);
}

BeginUnit(Transform)
    BeginComponent(Transform)
        RegisterBase(SceneNode)
        RegisterProperty(v3f, Position3D)
        RegisterProperty(v3f, Scale3D)
        RegisterProperty(v3f, RotationEuler3D)
        RegisterPropertyReadOnly(v4f, RotationQuat3D)
        RegisterProperty(v2f, Position2D)
        RegisterProperty(v2f, Scale2D)
        RegisterProperty(float, Rotation2D)
        RegisterProperty(float, Distance2D)

        defaultValue.Scale3D = {1.0f, 1.0f, 1.0f};
        defaultValue.RotationQuat3D = {0.0f, 0.0f, 0.0f, 1.0f};
    EndComponent()

    RegisterDeferredSystem(OnTransformChanged, ComponentOf_Transform(), AppLoopOrder_TransformUpdate)
    RegisterDeferredSystem(OnOwnerChanged, ComponentOf_Ownership(), AppLoopOrder_TransformUpdate * 0.99f)
EndUnit()
