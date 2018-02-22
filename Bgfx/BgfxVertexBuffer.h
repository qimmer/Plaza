//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXVERTEXBUFFER_H
#define PLAZA_BGFXVERTEXBUFFER_H

#include <Core/Service.h>
#include <Core/Entity.h>


    DeclareComponent(BgfxVertexBuffer)
    DeclareComponent(BgfxVertexDeclaration)
    DeclareService(BgfxVertexBuffer)

    u16 GetBgfxVertexBufferHandle(Entity entity);
    void UpdateBgfxVertexBuffer(Entity entity);
    void UpdateBgfxVertexDeclaration(Entity entity);


#endif //PLAZA_BGFXVERTEXBUFFER_H
