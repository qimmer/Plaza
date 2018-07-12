//
// Created by Kim Johannsen on 09-03-2018.
//

#include "VertexDeclaration.h"
#include "VertexAttribute.h"

struct VertexDeclaration {
};

BeginUnit(VertexDeclaration)
    BeginComponent(VertexDeclaration)
    RegisterBase(Node)
EndComponent()

API_EXPORT u32 GetVertexStride(Entity vertexDeclaration) {
    u32 stride = 0;
    for(auto attribute = GetFirstChild(vertexDeclaration); IsEntityValid(attribute); attribute = GetSibling(attribute)) {
        if(HasComponent(attribute, ComponentOf_VertexAttribute())) {
            stride += GetTypeSize(GetVertexAttributeType(attribute));
        }
    }
    return stride;
}

LocalFunction(OnChanged, void, Entity entity) {
    if(HasComponent(entity, ComponentOf_VertexAttribute())) {
        auto vertexDeclaration = GetParent(entity);
        if(IsEntityValid(vertexDeclaration) && HasComponent(vertexDeclaration, ComponentOf_VertexDeclaration())) {
            FireEvent(EventOf_VertexDeclarationChanged(), vertexDeclaration);
        }
    }
}

DefineService(VertexDeclaration)
        RegisterSubscription(VertexAttributeChanged, OnChanged, 0)
        RegisterSubscription(VertexAttributeRemoved, OnChanged, 0)
EndService()
