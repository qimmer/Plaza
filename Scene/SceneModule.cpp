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
#include "Scene.h"
#include "SceneNode.h"
#include "Transform.h"

BeginModule(Scene)
    RegisterUnit(Camera)
    RegisterUnit(Frustum)
    RegisterUnit(Light)
    RegisterUnit(Model)
    RegisterUnit(OrthographicFrustum)
    RegisterUnit(PerspectiveFrustum)
    RegisterUnit(Scene)
    RegisterUnit(SceneNode)
    RegisterUnit(Transform)
EndModule()
