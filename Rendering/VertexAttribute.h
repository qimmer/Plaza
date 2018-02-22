//
// Created by Kim Johannsen on 14/01/2018.
//

#ifndef PLAZA_VERTEXATTRIBUTE_H
#define PLAZA_VERTEXATTRIBUTE_H


#include <Core/Entity.h>


    DeclareComponent(VertexAttribute)

    DeclareComponentProperty(VertexAttribute, Type, VertexAttributeType)
    DeclareComponentProperty(VertexAttribute, u8, VertexAttributeUsage)
    DeclareComponentProperty(VertexAttribute, bool, VertexAttributeNormalize)
    DeclareComponentProperty(VertexAttribute, bool, VertexAttributeAsInt)

    enum
    {
        VertexAttributeUsage_Position,
        VertexAttributeUsage_Normal,
        VertexAttributeUsage_Tangent,
        VertexAttributeUsage_Bitangent,
        VertexAttributeUsage_Color0,
        VertexAttributeUsage_Color1,
        VertexAttributeUsage_Color2,
        VertexAttributeUsage_Color3,
        VertexAttributeUsage_Indices,
        VertexAttributeUsage_Weight,
        VertexAttributeUsage_TexCoord0,
        VertexAttributeUsage_TexCoord1,
        VertexAttributeUsage_TexCoord2,
        VertexAttributeUsage_TexCoord3,
        VertexAttributeUsage_TexCoord4,
        VertexAttributeUsage_TexCoord5,
        VertexAttributeUsage_TexCoord6,
        VertexAttributeUsage_TexCoord7
    };


#endif //PLAZA_VERTEXATTRIBUTE_H
