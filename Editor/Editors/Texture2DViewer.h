//
// Created by Kim Johannsen on 22-03-2018.
//

#ifndef PLAZA_TEXTURE2DVIEWER_H
#define PLAZA_TEXTURE2DVIEWER_H

#include <Core/NativeUtils.h>


Unit(Texture2DViewer)
    Component(Texture2DViewer)
        Property(Entity, Texture2DViewerTexture)

void ViewTexture(Entity texture);

#endif //PLAZA_TEXTURE2DVIEWER_H
