//
// Created by Kim Johannsen on 23/01/2018.
//

#include <Foundation/Stream.h>
#include "Folder.h"
#include <Foundation/VirtualPath.h>

#undef Enum

#include <unistd.h>
#include <dirent.h>

#include <algorithm>
#include <climits>

#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <iterator>
#include <Core/Identification.h>
#include <Core/Debug.h>

struct Folder {
    ChildArray FolderSubfolders, FolderFiles;
    StringRef FolderPath;
};

API_EXPORT void ScanFolder(Entity entity) {
    auto folderData = GetFolder(entity);
    folderData.FolderFiles.SetSize(0);

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (GetFolder(entity).FolderPath)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            Entity entryEntity = 0;

            if(ent->d_name[0] == '.') continue;

            char entryPath[PathMax];
            snprintf(entryPath, PathMax, "%s/%s", GetFolder(entity).FolderPath, ent->d_name);

            struct stat ent_stat;
            stat(entryPath, &ent_stat);

            switch(ent_stat.st_mode & S_IFMT) {
                case S_IFDIR:
                {
                    entryEntity = CreateEntity();
                    //SetName(entryEntity, ent->d_name);
                    auto subFolderData = GetFolder(entryEntity);
                    subFolderData.FolderPath = entryPath;
                    SetFolder(entryEntity, subFolderData);
                    folderData.FolderSubfolders.Add(entryEntity);
                    break;
                }
                case S_IFREG:
                {
                    entryEntity = CreateEntity();
                    //SetName(entryEntity, ent->d_name);
                    auto streamData = GetStream(entity);
                    streamData.StreamPath = entryPath;
                    SetStream(entryEntity, streamData);
                    folderData.FolderFiles.Add(entryEntity);
                    break;
                }
            }
        }
        closedir (dir);
    }
}

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#undef GetHandle
#include <windows.h>
#include <shlwapi.h>
#include <Core/Debug.h>

#undef CreateService
#undef CreateEvent
#endif

API_EXPORT bool CreateDirectories(StringRef fullPath) {
    auto isVirtualPath = strstr(fullPath, "://") != NULL;

    if(isVirtualPath && memcmp(fullPath, "file://", 7) != 0) {
        Log(0, LogSeverity_Error, "CreateDirectory only supports file:// paths.");
        return false;
    }

    if(isVirtualPath) {
        fullPath += 7;
    }

#ifdef WIN32
    if (!(CreateDirectoryA(fullPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
#else
    if(mkdir(fullPath, 0700))
#endif
    {
        Log(0, LogSeverity_Error, "Could not create directory '%s'", fullPath);
        return false;
    }

    return true;
}

API_EXPORT bool IsFolder(StringRef absolutePath) {
    auto resolvedPath = ResolveVirtualPath(absolutePath);

    auto isVirtualPath = strstr(resolvedPath, "://") != NULL;

    if(isVirtualPath && memcmp(resolvedPath, "file://", 7) != 0) {
        Log(0, LogSeverity_Error, "IsDirectory only supports file:// paths.");
        return false;
    }

    resolvedPath += 7;

    struct stat ent_stat;
    stat(resolvedPath, &ent_stat);

    if(ent_stat.st_mode & S_IFMT) return true;

    return false;
}

BeginUnit(Folder)
    BeginComponent(Folder)
        RegisterProperty(StringRef, FolderPath)
    EndComponent()
EndComponent()
