//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_FILESTREAM_H
#define PLAZA_FILESTREAM_H

#include <Core/NativeUtils.h>

Unit(FileStream)
    Component(FileStream)

Function(GetNativePath, StringRef, StringRef resourcePath);

Declare(AppLoop, FileChangesPoll)

#endif //PLAZA_FILESTREAM_H
