//
// Created by Kim on 04-10-2018.
//

#ifndef PLAZA_MESHBUILDER_H
#define PLAZA_MESHBUILDER_H

#include <Core/NativeUtils.h>

struct MeshBuilderVertex {
    v3f MeshBuilderVertexPosition, MeshBuilderVertexNormal;
    v2f MeshBuilderVertexTexCoord0, MeshBuilderVertexTexCoord1;
    rgba8 MeshBuilderVertexColor0;
};

struct MeshBuilderIndex {
    u32 MeshBuilderIndexVertexIndex;
};

struct MeshBuilder {
};

Unit(MeshBuilder)
    Component(MeshBuilderVertex)
        Property(v3f, MeshBuilderVertexPosition)
        Property(v3f, MeshBuilderVertexNormal)
        Property(v2f, MeshBuilderVertexTexCoord0)
        Property(v2f, MeshBuilderVertexTexCoord1)
        Property(rgba8, MeshBuilderVertexColor0)

    Component(MeshBuilderIndex)
        Property(u32, MeshBuilderIndexVertexIndex)

    Component(MeshBuilder)
        ArrayProperty(MeshBuilderVertex, MeshBuilderVertices)
        ArrayProperty(MeshBuilderIndex, MeshBuilderIndices)

#endif //PLAZA_MESHBUILDER_H
