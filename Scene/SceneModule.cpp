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
#include "Transform2D.h"
#include "Transform3D.h"
#include "Scene.h"

BeginModule(Scene)
    RegisterUnit(Transform2D)
    RegisterUnit(Transform3D)
    RegisterUnit(Transform)
    RegisterUnit(Camera)
    RegisterUnit(OrthographicFrustum)
    RegisterUnit(PerspectiveFrustum)
    RegisterUnit(Frustum)
    RegisterUnit(Light)
    RegisterUnit(Model)
    RegisterUnit(Scene)
EndModule()
