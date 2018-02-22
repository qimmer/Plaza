//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_FILESTREAM_H
#define PLAZA_FILESTREAM_H

#include <Core/Entity.h>

#define FileMode_Read 1
#define FileMode_Write 2

DeclareComponent(FileStream)

DeclareComponentProperty(FileStream, StringRef, FilePath)
DeclareComponentProperty(FileStream, int, FileMode)

DeclareService(FileStream)

void RegisterFileType(StringRef fileExtension, StringRef mimeType);

#endif //PLAZA_FILESTREAM_H
