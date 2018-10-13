//
// Created by Kim on 04-10-2018.
//

#ifndef PLAZA_MESHBUILDER_H
#define PLAZA_MESHBUILDER_H

#include <Core/NativeUtils.h>

Unit(MeshBuilder)
    Component(MeshBuilderVertex)
        Property(v3f, MeshBuilderVertexPosition)
        Property(v3f, MeshBuilderVertexNormal)
        Property(v2f, MeshBuilderVertexTexCoord0)
        Property(rgba8, MeshBuilderVertexColor0)

    Component(MeshBuilderIndex)
        Property(u32, MeshBuilderIndexVertexIndex)

    Component(MeshBuilder)
        ArrayProperty(MeshBuilderVertex, MeshBuilderVertices)
        ArrayProperty(MeshBuilderIndex, MeshBuilderIndices)

#endif //PLAZA_MESHBUILDER_H
