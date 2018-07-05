//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Transform.h>
#include "Transform2D.h"

#include <cglm/cglm.h>

struct Transform2D {
    Transform2D() : Scale2D({1.0f, 1.0f}) {
    }
    v2f Position2D, Scale2D;
    float Rotation2D, Distance2D;
};

DefineComponent(Transform2D)
    Dependency(Transform)
    DefinePropertyReactive(v2f, Position2D)
    DefinePropertyReactive(v2f, Scale2D)
    DefinePropertyReactive(float, Rotation2D)
    DefinePropertyReactive(float, Distance2D)
EndComponent()

DefineComponentPropertyReactive(Transform2D, v2f, Position2D)
DefineComponentPropertyReactive(Transform2D, float, Rotation2D)
DefineComponentPropertyReactive(Transform2D, v2f, Scale2D)
DefineComponentPropertyReactive(Transform2D, float, Distance2D)

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

    SetLocalTransform(entity, m);
}

static void Onv2fChanged(Entity entity, v2f oldValue, v2f newValue) {
    if(memcmp(&oldValue, &newValue, sizeof(v2f)) != 0) {
        UpdateLocalTransform(entity);
    }
}

static void OnFloatChanged(Entity entity, float oldValue, float newValue) {
    if(oldValue != newValue) {
        UpdateLocalTransform(entity);
    }
}

DefineService(Transform2D)
        Subscribe(Position2DChanged, Onv2fChanged)
        Subscribe(Scale2DChanged, Onv2fChanged)
        Subscribe(Rotation2DChanged, OnFloatChanged)
        Subscribe(Distance2DChanged, OnFloatChanged)
EndService()
