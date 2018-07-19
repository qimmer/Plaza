//
// Created by Kim Johannsen on 23/01/2018.
//

#include <Foundation/Stream.h>
#include "Folder.h"
#include <Foundation/VirtualPath.h>

#include <unistd.h>
#include <algorithm>
#include <climits>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <iterator>
#include <Core/Identification.h>

struct Folder {
    char FolderPath[PATH_MAX];
    Vector(FolderSubfolders, Entity, 32);
    Vector(FolderFiles, Entity, 128);
};

API_EXPORT void ScanFolder(Entity entity) {
    while(GetNumFolderFiles(entity)) {
        RemoveFolderFiles(entity, 0);
    }

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (GetFolderPath(entity))) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            Entity entryEntity = 0;

            if(ent->d_name[0] == '.') continue;

            char entryPath[PathMax];
            snprintf(entryPath, PathMax, "%s/%s", GetFolderPath(entity), ent->d_name);

            struct stat ent_stat;
            stat(entryPath, &ent_stat);

            switch(ent_stat.st_mode & S_IFMT) {
                case S_IFDIR:
                    entryEntity = AddFolderSubfolders(entity);
                    SetName(entryEntity, ent->d_name);
                    SetFolderPath(entryEntity, entryPath);
                    break;
                case S_IFREG:
                    entryEntity = AddFolderFiles(entity);
                    SetName(entryEntity, ent->d_name);
                    SetStreamPath(entryEntity, entryPath);
                    break;
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
    char resolvedPath[PathMax];
    ResolveVirtualPath(absolutePath, PathMax, resolvedPath);

    auto isVirtualPath = strstr(resolvedPath, "://") != NULL;

    if(isVirtualPath && memcmp(resolvedPath, "file://", 7) != 0) {
        Log(0, LogSeverity_Error, "IsDirectory only supports file:// paths.");
        return false;
    }

    char *rawPath = resolvedPath;
    rawPath += 7;

    struct stat ent_stat;
    stat(rawPath, &ent_stat);

    if(ent_stat.st_mode & S_IFMT) return true;

    return false;
}

LocalFunction(OnFolderPathChanged, void, Entity entity, StringRef before, StringRef after) {
    //SetName(entity, GetFileName(after));
}

LocalFunction(OnFolderAdded, void, Entity component, Entity entity) {

}

LocalFunction(OnFolderRemoved, void, Entity component, Entity entity) {

}

BeginUnit(Folder)
    BeginComponent(Folder)
        RegisterProperty(StringRef, FolderPath)
    EndComponent()

    RegisterSubscription(EntityComponentAdded, OnFolderAdded, ComponentOf_Folder())
    RegisterSubscription(EntityComponentRemoved, OnFolderRemoved, ComponentOf_Folder())
    RegisterSubscription(FolderPathChanged, OnFolderPathChanged, 0)
EndComponent()
