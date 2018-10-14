//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Transform.h>
#include "Transform2D.h"

#include <cglm/cglm.h>

struct Transform2D {
    v2f Position2D, Scale2D;
    float Rotation2D, Distance2D;
};

static void UpdateLocalTransform(Entity entity) {
    auto t = GetPosition2D(entity);
    auto r = GetRotation2D(entity);
    auto s = GetScale2D(entity);
    auto d = GetDistance2D(entity);

    static float rotAxis[] = {0.0f, 0.0f, 1.0f};
    float scale3D[] = {s.x, s.y, 1.0f};
    float trans3D[] = {t.x, t.y, d};

    m4x4f m = m4x4f_Identity;
    glm_translate((vec4*)&m.x.x, trans3D);
    glm_scale((vec4*)&m.x.x, scale3D);
    glm_rotate((vec4*)&m.x.x, glm_rad(r), rotAxis);

    SetTransformLocalMatrix(entity, m);
}

LocalFunction(Onv2fChanged, void, Entity entity, v2f oldValue, v2f newValue) {
    if(memcmp(&oldValue, &newValue, sizeof(v2f)) != 0) {
        UpdateLocalTransform(entity);
    }
}

LocalFunction(OnFloatChanged, void, Entity entity, float oldValue, float newValue) {
    if(oldValue != newValue) {
        UpdateLocalTransform(entity);
    }
}

LocalFunction(OnAdded, void, Entity component, Entity entity) {
    SetScale2D(entity, {1.0f, 1.0f});
}

BeginUnit(Transform2D)
    BeginComponent(Transform2D)
        RegisterBase(Transform)
        RegisterProperty(v2f, Position2D)
        RegisterProperty(v2f, Scale2D)
        RegisterProperty(float, Rotation2D)
        RegisterProperty(float, Distance2D)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Position2D()), Onv2fChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Scale2D()), Onv2fChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Rotation2D()), OnFloatChanged, 0)
    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_Distance2D()), OnFloatChanged, 0)
    RegisterSubscription(EventOf_EntityComponentAdded(), OnAdded, ComponentOf_Transform2D())
EndUnit()
