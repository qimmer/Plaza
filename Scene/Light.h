//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_LIGHT_H
#define PLAZA_LIGHT_H

#include <Core/NativeUtils.h>

Unit(Light)
    Component(Light)
        Property(rgba8, LightColor)
        Property(float, LightIntensity)
        Property(bool, LightCastShadows)

    Component(DirectionalLight)

    Component(SpotLight)
        Property(float, SpotLightDistance)
        Property(float, SpotLightFieldOfView)

    Component(PointLight)
        Property(float, PointLightRadius)

#endif //PLAZA_CAMERA_H
