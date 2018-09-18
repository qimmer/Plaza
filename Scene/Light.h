//
// Created by Kim Johannsen on 27/01/2018.
//

#ifndef PLAZA_LIGHT_H
#define PLAZA_LIGHT_H

#include <Core/NativeUtils.h>

Unit(Light)
    Component(Light)
        Property(rgba16, LightColor)
        Property(float, LightIntensity)
        Property(bool, LightRenderShadows)
        ChildProperty(OffscreenRenderTarget, LightShadowMapTarget)

    Component(SpotLight)
        Property(u8, DirectionalLightShadowSplits)

    Component(DirectionalLight)
        Property(u8, DirectionalLightShadowSplits)

    Component(PointLight)

#endif //PLAZA_CAMERA_H
