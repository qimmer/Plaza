//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXPROGRAM_H
#define PLAZA_BGFXPROGRAM_H

#include <Core/Service.h>
#include <Core/Entity.h>

DeclareComponent(BgfxProgram)
DeclareService(BgfxProgram)

u16 GetBgfxProgramHandle(Entity entity);

#endif //PLAZA_BGFXPROGRAM_H
