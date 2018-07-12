//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXSHADER_H
#define PLAZA_BGFXSHADER_H

#include <Core/NativeUtils.h>


    Unit(BgfxBinaryShader)
    Component(BgfxBinaryShader)
    DeclareService(BgfxBinaryShader)

    u16 GetBgfxBinaryShaderHandle(Entity entity);

    void UpdateBgfxBinaryShader(Entity entity);


#endif //PLAZA_BGFXSHADER_H
