//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/Hierarchy.h>
#include "VertexAttribute.h"

struct VertexAttribute {
    Type VertexAttributeType;
    u8 VertexAttributeUsage;
    bool VertexAttributeNormalize, VertexAttributeAsInt;
};

DefineComponent(VertexAttribute)
    Dependency(Hierarchy)
    DefineProperty(Type, VertexAttributeType)
    DefineProperty(u8, VertexAttributeUsage)
    DefineProperty(bool, VertexAttributeNormalize)
    DefineProperty(bool, VertexAttributeAsInt)
EndComponent()

DefineComponentPropertyReactive(VertexAttribute, Type, VertexAttributeType)
DefineComponentPropertyReactive(VertexAttribute, u8, VertexAttributeUsage)
DefineComponentPropertyReactive(VertexAttribute, bool, VertexAttributeNormalize)
DefineComponentPropertyReactive(VertexAttribute, bool, VertexAttributeAsInt)

