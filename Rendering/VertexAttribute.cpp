//
// Created by Kim Johannsen on 14/01/2018.
//

#include <Core/Node.h>
#include "VertexAttribute.h"

struct VertexAttribute {
    Type VertexAttributeType;
    u8 VertexAttributeUsage;
    bool VertexAttributeNormalize, VertexAttributeAsInt;
};

BeginUnit(VertexAttribute)
    BeginComponent(VertexAttribute)
    RegisterBase(Node)
    RegisterProperty(Type, VertexAttributeType)
    RegisterProperty(u8, VertexAttributeUsage)
    RegisterProperty(bool, VertexAttributeNormalize)
    RegisterProperty(bool, VertexAttributeAsInt)
EndComponent()

RegisterProperty(Type, VertexAttributeType)
RegisterProperty(u8, VertexAttributeUsage)
RegisterProperty(bool, VertexAttributeNormalize)
RegisterProperty(bool, VertexAttributeAsInt)

