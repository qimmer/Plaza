//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXCOMMANDLIST_H
#define PLAZA_BGFXCOMMANDLIST_H

#include <Core/Service.h>
#include <Core/Entity.h>


    DeclareComponent(BgfxCommandList)
    DeclareService(BgfxCommandList)

    void RenderCommandList(Entity commandList);


#endif //PLAZA_BGFXCOMMANDLIST_H
