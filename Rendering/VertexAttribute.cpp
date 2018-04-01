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
    DefinePropertyReactive(Type, VertexAttributeType)
    DefinePropertyReactive(u8, VertexAttributeUsage)
    DefinePropertyReactive(bool, VertexAttributeNormalize)
    DefinePropertyReactive(bool, VertexAttributeAsInt)
EndComponent()

DefineComponentPropertyReactive(VertexAttribute, Type, VertexAttributeType)
DefineComponentPropertyReactive(VertexAttribute, u8, VertexAttributeUsage)
DefineComponentPropertyReactive(VertexAttribute, bool, VertexAttributeNormalize)
DefineComponentPropertyReactive(VertexAttribute, bool, VertexAttributeAsInt)

