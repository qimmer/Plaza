//
// Created by Kim Johannsen on 15/01/2018.
//

#ifndef PLAZA_BGFXMODULE_H
#define PLAZA_BGFXMODULE_H


#include <Core/Module.h>

Module(BgfxRendering)

Unit(BgfxRendering)
    Component(BgfxRendering)
        ChildProperty(AppLoop, BgfxRenderingLoop)

#endif //PLAZA_BGFXMODULE_H
