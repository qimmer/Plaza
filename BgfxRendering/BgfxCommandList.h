//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_BGFXCOMMANDLIST_H
#define PLAZA_BGFXCOMMANDLIST_H

#include <Core/NativeUtils.h>

Unit(BgfxCommandList)
    Component(BgfxCommandList)

void RenderCommandList(Entity commandList, unsigned char viewId);

#endif //PLAZA_BGFXCOMMANDLIST_H
