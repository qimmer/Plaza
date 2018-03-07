//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/Hierarchy.h>
#include <Foundation/Invalidation.h>
#include "VertexAttribute.h"


    struct VertexAttribute {
        VertexAttribute() {
            memset(this, 0, sizeof(VertexAttribute));
        }
        Type VertexAttributeType;
        u8 VertexAttributeUsage;
        bool VertexAttributeNormalize, VertexAttributeAsInt;
    };

    DefineComponent(VertexAttribute)
        Dependency(Hierarchy)
        Dependency(Invalidation)
        DefineProperty(Type, VertexAttributeType)
        DefineProperty(u8, VertexAttributeUsage)
        DefineProperty(bool, VertexAttributeNormalize)
        DefineProperty(bool, VertexAttributeAsInt)
    EndComponent()

    DefineComponentProperty(VertexAttribute, Type, VertexAttributeType)
    DefineComponentProperty(VertexAttribute, u8, VertexAttributeUsage)
    DefineComponentProperty(VertexAttribute, bool, VertexAttributeNormalize)
    DefineComponentProperty(VertexAttribute, bool, VertexAttributeAsInt)

