//
// Created by Kim Johannsen on 01/02/2018.
//

#include <Rendering/RenderingModule.h>

#include "Camera.h"
#include "Frustum.h"
#include "Light.h"
#include "Model.h"
#include "OrthographicFrustum.h"
#include "PerspectiveFrustum.h"
#include "Transform.h"
#include "Scene.h"
#include "Ray.h"

BeginModule(Scene)
    RegisterUnit(Transform)
    RegisterUnit(Camera)
    RegisterUnit(OrthographicFrustum)
    RegisterUnit(PerspectiveFrustum)
    RegisterUnit(Frustum)
    RegisterUnit(Light)
    RegisterUnit(Model)
    RegisterUnit(Scene)
    RegisterUnit(Ray)
EndModule()
