//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXCONTEXT_H
#define PLAZA_BGFXCONTEXT_H

#include <Core/Entity.h>
#include <Core/Service.h>

DeclareComponent(BgfxContext)
DeclareService(BgfxContext)

u16 GetBgfxContextHandle(Entity entity);

#endif //PLAZA_BGFXCONTEXT_H
