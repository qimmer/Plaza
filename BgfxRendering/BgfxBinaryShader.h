//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXSHADER_H
#define PLAZA_BGFXSHADER_H

#include <Core/Service.h>
#include <Core/Entity.h>


    DeclareComponent(BgfxBinaryShader)
    DeclareService(BgfxBinaryShader)

    u16 GetBgfxBinaryShaderHandle(Entity entity);

    void UpdateBgfxBinaryShader(Entity entity);


#endif //PLAZA_BGFXSHADER_H
