//
// Created by Kim Johannsen on 23/01/2018.
//

#ifndef PLAZA_FILEEXPLORER_H
#define PLAZA_FILEEXPLORER_H

#include <Core/Service.h>
#include <Core/Entity.h>


DeclareService(FileExplorer)

bool GetFileExplorerVisible();
void SetFileExplorerVisible(bool value);

namespace ImGui {
    void FileStreamContextMenu(Entity stream);
}

#endif //PLAZA_FILEEXPLORER_H
