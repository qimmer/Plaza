//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_FILESTREAM_H
#define PLAZA_FILESTREAM_H

#include <Core/Entity.h>

DeclareComponent(FileStream)
DeclareService(FileStream)

StringRef GetNativePath(StringRef resourcePath);

#endif //PLAZA_FILESTREAM_H
