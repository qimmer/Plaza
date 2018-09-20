
#include "Light.h"

struct Light {
    rgba8 LightColor;
    float LightIntensity;
    bool LightCastShadows;
};

struct DirectionalLight {

};

struct SpotLight {
    float SpotLightDistance, SpotLightFieldOfView;
};

struct PointLight {
    float PointLightRadius;
};

BeginUnit(Light)
    BeginComponent(Light)
        RegisterProperty(rgba8, LightColor)
        RegisterProperty(float, LightIntensity)
        RegisterProperty(bool, LightCastShadows)
    EndComponent()

    BeginComponent(DirectionalLight)
    EndComponent()

    BeginComponent(SpotLight)
        RegisterProperty(float, SpotLightDistance)
        RegisterProperty(float, SpotLightFieldOfView)
    EndComponent()

    BeginComponent(PointLight)
        RegisterProperty(float, PointLightRadius)
    EndComponent()
EndUnit()