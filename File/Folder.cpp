//
// Created by Kim Johannsen on 23/01/2018.
//

#include <Core/String.h>
#include <Foundation/Stream.h>
#include "Folder.h"
#include "Core/Hierarchy.h"
#include <Foundation/VirtualPath.h>

#include <unistd.h>
#include <algorithm>
#include <climits>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <iterator>

struct Folder {
    String FolderPath;
};

DefineComponent(Folder)
    Dependency(Hierarchy)
EndComponent()

DefineComponentPropertyReactive(Folder, StringRef, FolderPath)

void ScanFolder(Entity entity) {
    while(IsEntityValid(GetFirstChild(entity))) {
        DestroyEntity(GetFirstChild(entity));
    }

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (GetFolderPath(entity))) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            Entity entryEntity = 0;

            if(ent->d_name[0] == '.') continue;

            char entryPath[PATH_MAX];
            snprintf(entryPath, PATH_MAX, "%s/%s", GetFolderPath(entity), ent->d_name);

            char entityPath[PATH_MAX];
            snprintf(entityPath, PATH_MAX, "%s/%s", GetEntityPath(entity), ent->d_name);

            struct stat ent_stat;
            stat(entryPath, &ent_stat);

            switch(ent_stat.st_mode & S_IFMT) {
                case S_IFDIR:
                    entryEntity = CreateFolder(entity, ent->d_name);
                    SetName(entryEntity, ent->d_name);
                    SetParent(entryEntity, entity);
                    SetFolderPath(entryEntity, entryPath);
                    break;
                case S_IFREG:
                    entryEntity = CreateStream(entity, ent->d_name);
                    SetName(entryEntity, ent->d_name);
                    SetParent(entryEntity, entity);
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
#undef CreateService
#undef CreateEvent
#endif

API_EXPORT bool CreateDirectories(StringRef fullPath) {
    auto isVirtualPath = strstr(fullPath, "://") != NULL;

    if(isVirtualPath && memcmp(fullPath, "file://", 7) != 0) {
        Log(LogChannel_Core, LogSeverity_Error, "CreateDirectory only supports file:// paths.");
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
        Log(LogChannel_Core, LogSeverity_Error, "Could not create directory '%s'", fullPath);
        return false;
    }

    return true;
}

API_EXPORT bool IsFolder(StringRef absolutePath) {
    char resolvedPath[PATH_MAX];
    ResolveVirtualPath(absolutePath, resolvedPath);

    auto isVirtualPath = strstr(resolvedPath, "://") != NULL;

    if(isVirtualPath && memcmp(resolvedPath, "file://", 7) != 0) {
        Log(LogChannel_Core, LogSeverity_Error, "IsDirectory only supports file:// paths.");
        return false;
    }

    char *rawPath = resolvedPath;
    rawPath += 7;

    struct stat ent_stat;
    stat(rawPath, &ent_stat);

    if(ent_stat.st_mode & S_IFMT) return true;

    return false;
}

static void OnFolderPathChanged(Entity entity, StringRef before, StringRef after) {
    //SetName(entity, GetFileName(after));
}

static void OnFolderAdded(Entity entity) {

}

static void OnFolderRemoved(Entity entity) {

}

DefineService(Folder)
        ServiceDependency(Stream)
        Subscribe(FolderAdded, OnFolderAdded)
        Subscribe(FolderRemoved, OnFolderRemoved)
        Subscribe(FolderPathChanged, OnFolderPathChanged)
EndService()
