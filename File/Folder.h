//
// Created by Kim Johannsen on 23/01/2018.
//

#ifndef PLAZA_FOLDER_H
#define PLAZA_FOLDER_H

#include <Core/NativeUtils.h>

Unit(Folder)
    Component(Folder)
        Property(StringRef, FolderPath)

Function(IsFolder, bool, StringRef absolutePath)
Function(CreateDirectories, bool, StringRef fullPath)
Function(ScanFolder, void, Entity folder)


#endif //PLAZA_FOLDER_H
