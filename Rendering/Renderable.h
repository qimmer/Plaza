//
// Created by Kim on 20-09-2018.
//

#ifndef PLAZA_RENDERABLE_H
#define PLAZA_RENDERABLE_H

#include <Core/NativeUtils.h>

struct Renderable {
    Entity RenderableMaterial, RenderableSubMesh;
    v3f RenderableAABBMin, RenderableAABBMax;
};

Unit(Renderable)

Component(Renderable)
    ReferenceProperty(Material, RenderableMaterial)
    ReferenceProperty(SubMesh, RenderableSubMesh)
    Property(v3f, RenderableAABBMin)
    Property(v3f, RenderableAABBMax)

	Declare(AppLoop, BoundsUpdate)

#define AppLoopOrder_BoundsUpdate (AppLoopOrder_TransformUpdate + 1.0f)

#endif //PLAZA_RENDERABLE_H
