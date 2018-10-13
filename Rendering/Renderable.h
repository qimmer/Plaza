//
// Created by Kim on 20-09-2018.
//

#ifndef PLAZA_RENDERABLE_H
#define PLAZA_RENDERABLE_H

#include <Core/NativeUtils.h>

Unit(Renderable)

Component(Renderable)
    ReferenceProperty(Material, RenderableMaterial)
    ReferenceProperty(SubMesh, RenderableSubMesh)

#endif //PLAZA_RENDERABLE_H
