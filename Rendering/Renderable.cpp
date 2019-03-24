//
// Created by Kim on 20-09-2018.
//

#include "Renderable.h"
#include "Material.h"
#include "Mesh.h"
#include "Uniform.h"
#include <Scene/Transform.h>
#include <Scene/Scene.h>
#include <Foundation/Visibility.h>

BeginUnit(Renderable)
    BeginComponent(Renderable)
        RegisterBase(SceneNode)
        RegisterBase(Transform)
        RegisterBase(Visibility)
        RegisterReferenceProperty(Material, RenderableMaterial)
        RegisterReferenceProperty(SubMesh, RenderableSubMesh)
        RegisterProperty(v3f, RenderableAABBMin)
        RegisterProperty(v3f, RenderableAABBMax)
        RegisterProperty(v4i, RenderableScissor)
    EndComponent()

	SetAppLoopOrder(AppLoopOf_BoundsUpdate(), AppLoopOrder_BoundsUpdate);
EndUnit()