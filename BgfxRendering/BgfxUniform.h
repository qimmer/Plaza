//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXUNIFORM_H
#define PLAZA_BGFXUNIFORM_H

#include <Core/Service.h>
#include <Core/Entity.h>


DeclareComponent(BgfxUniform)
DeclareService(BgfxUniform)

u16 GetBgfxUniformHandle(Entity entity);

#endif //PLAZA_BGFXUNIFORM_H
