//
// Created by Kim Johannsen on 28/01/2018.
//

#include <Scene/Transform.h>
#include "Transform2D.h"


    struct Transform2D {
        Transform2D() : Scale2D({1.0f, 1.0f}) {
        }
        v2f Position2D, Scale2D;
        float Rotation2D;
    };

    DefineComponent(Transform2D)
        Dependency(Transform)
        DefineProperty(v2f, Position2D)
        DefineProperty(v2f, Scale2D)
        DefineProperty(float, Rotation2D)
    EndComponent()

    DefineService(Transform2D)
    EndService()

    DefineComponentProperty(Transform2D, v2f, Position2D)
    DefineComponentProperty(Transform2D, float, Rotation2D)
    DefineComponentProperty(Transform2D, v2f, Scale2D)

    static bool ServiceStart() {
        return true;
    }

    static bool ServiceStop() {
        return true;
    }

