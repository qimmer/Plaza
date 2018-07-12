//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXINDEXBUFFER_H
#define PLAZA_BGFXINDEXBUFFER_H

#include <Core/NativeUtils.h>


    Unit(BgfxIndexBuffer)
    Component(BgfxIndexBuffer)
    DeclareService(BgfxIndexBuffer)

    u16 GetBgfxIndexBufferHandle(Entity entity);

    void UpdateBgfxIndexBuffer(Entity entity);

#endif //PLAZA_BGFXINDEXBUFFER_H
