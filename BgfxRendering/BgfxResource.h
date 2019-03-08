//
// Created by Kim on 22-08-2018.
//

#ifndef PLAZA_BGFXRESOURCE_H
#define PLAZA_BGFXRESOURCE_H

#include <Core/NativeUtils.h>

struct BgfxResource {
	u16 BgfxResourceHandle;
};

Unit(BgfxResource)
    Component(BgfxResource)
        Property(u16, BgfxResourceHandle)

#endif //PLAZA_BGFXRESOURCE_H
