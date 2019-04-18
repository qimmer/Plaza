//
// Created by Kim Johannsen on 13/01/2018.
//

#ifndef PLAZA_FILESTREAM_H
#define PLAZA_FILESTREAM_H

#include <Core/NativeUtils.h>
#include <Foundation/NativeUtils.h>

Unit(FileStream)
    Component(FileStream)
    StreamProtocol(File)

Function(GetNativePath, StringRef, StringRef resourcePath);

#endif //PLAZA_FILESTREAM_H
