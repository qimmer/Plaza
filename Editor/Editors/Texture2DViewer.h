//
// Created by Kim Johannsen on 22-03-2018.
//

#ifndef PLAZA_TEXTURE2DVIEWER_H
#define PLAZA_TEXTURE2DVIEWER_H

#include <Core/Entity.h>

DeclareService(Texture2DViewer)

DeclareComponent(Texture2DViewer)
DeclareComponentPropertyReactive(Texture2DViewer, Entity, Texture2DViewerTexture)

void ViewTexture(Entity texture);

#endif //PLAZA_TEXTURE2DVIEWER_H
