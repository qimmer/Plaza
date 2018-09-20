//
// Created by Kim on 20-09-2018.
//

#ifndef PLAZA_RENDERABLE_H
#define PLAZA_RENDERABLE_H

#include <Core/NativeUtils.h>

Unit(Renderable)

///
/// The Renderable component identifies a rendered instance in a context, such as a scene etc.
/// A renderable can be rendered multiple times by multiple batches pointing to the same renderable.
/// The renderable component stores rendering state that is common among all batches
///
Component(Renderable)
    Property(m4x4f, RenderableWorldMatrix)
    ArrayProperty(UniformState, RenderableUniformStates)
    ReferenceProperty(Material, RenderableMaterial)
    ReferenceProperty(SubMesh, RenderableSubMesh)

#endif //PLAZA_RENDERABLE_H
