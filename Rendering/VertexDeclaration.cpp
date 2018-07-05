//
// Created by Kim Johannsen on 09-03-2018.
//

#include "VertexDeclaration.h"
#include "VertexAttribute.h"

struct VertexDeclaration {
};

DefineComponent(VertexDeclaration)
    Dependency(Hierarchy)
EndComponent()

API_EXPORT u32 GetVertexStride(Entity vertexDeclaration) {
    u32 stride = 0;
    for(auto attribute = GetFirstChild(vertexDeclaration); IsEntityValid(attribute); attribute = GetSibling(attribute)) {
        if(HasVertexAttribute(attribute)) {
            stride += GetTypeSize(GetVertexAttributeType(attribute));
        }
    }
    return stride;
}

static void OnChanged(Entity entity) {
    if(HasVertexAttribute(entity)) {
        auto vertexDeclaration = GetParent(entity);
        if(IsEntityValid(vertexDeclaration) && HasVertexDeclaration(vertexDeclaration)) {
            FireNativeEvent(VertexDeclarationChanged, vertexDeclaration);
        }
    }
}

DefineService(VertexDeclaration)
        Subscribe(VertexAttributeChanged, OnChanged)
        Subscribe(VertexAttributeRemoved, OnChanged)
EndService()
