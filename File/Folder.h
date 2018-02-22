//
// Created by Kim Johannsen on 23/01/2018.
//

#ifndef PLAZA_FOLDER_H
#define PLAZA_FOLDER_H

#include <Core/Entity.h>
#include <Core/Service.h>

DeclareComponent(Folder)

DeclareComponentProperty(Folder, StringRef, FolderPath)

StringRef GetCurrentWorkingDirectory();
StringRef GetParentFolder(StringRef absolutePath);
StringRef GetFileName(StringRef absolutePath);
StringRef GetFileExtension(StringRef absolutePath);
char GetPathSeparator();
void ScanFolder(Entity folder);

DeclareService(Folder)

#endif //PLAZA_FOLDER_H
