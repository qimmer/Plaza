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

DefineService(VertexDeclaration)
EndService()

u32 GetVertexStride(Entity vertexDeclaration) {
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
            FireEvent(VertexDeclarationChanged, vertexDeclaration);
        }
    }
}

static bool ServiceStart() {
    SubscribeVertexAttributeChanged(OnChanged);
    SubscribeVertexAttributeRemoved(OnChanged);
    return true;
}

static bool ServiceStop() {
    UnsubscribeVertexAttributeChanged(OnChanged);
    UnsubscribeVertexAttributeRemoved(OnChanged);
    return true;
}