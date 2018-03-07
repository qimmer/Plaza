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

DefineService(Folder)
    ServiceDependency(Stream)
EndService()

DefineComponentProperty(Folder, StringRef, FolderPath)

StringRef GetCurrentWorkingDirectory() {
    static char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    return path;
}

StringRef GetFileName(StringRef absolutePath) {
    auto fileName = strrchr(absolutePath, '/');
    if(!fileName) return absolutePath;
    return fileName + 1;
}

StringRef GetFileExtension(StringRef absolutePath) {
    auto extension = strrchr(absolutePath, '.');
    if(!extension) return "";
    return extension;
}

StringRef GetParentFolder(StringRef absolutePath) {
    auto parentFolder = (char*)GetTempString(absolutePath);

    auto ptr = strrchr(parentFolder, '/');
    if(!ptr) {
        return "";
    }

    parentFolder[ptr - parentFolder] = '\0';

    return parentFolder;
}

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
            sprintf(entryPath, "%s/%s", GetFolderPath(entity), ent->d_name);

            char entityPath[PATH_MAX];
            sprintf(entityPath, "%s/%s", GetEntityPath(entity), ent->d_name);

            struct stat ent_stat;
            stat(entryPath, &ent_stat);

            switch(ent_stat.st_mode & S_IFMT) {
                case S_IFDIR:
                    entryEntity = CreateFolder(entityPath);
                    SetName(entryEntity, ent->d_name);
                    SetParent(entryEntity, entity);
                    SetFolderPath(entryEntity, entryPath);
                    break;
                case S_IFREG:
                    entryEntity = CreateStream(entityPath);
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
#include <windows.h>
#include <shlwapi.h>
#endif

bool CreateDirectories(StringRef fullPath) {
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

bool IsFolder(StringRef absolutePath) {
    absolutePath = ResolveVirtualPath(absolutePath);

    auto isVirtualPath = strstr(absolutePath, "://") != NULL;

    if(isVirtualPath && memcmp(absolutePath, "file://", 7) != 0) {
        Log(LogChannel_Core, LogSeverity_Error, "IsDirectory only supports file:// paths.");
        return false;
    }

    absolutePath += 7;

    struct stat ent_stat;
    stat(absolutePath, &ent_stat);

    if(ent_stat.st_mode & S_IFMT) return true;

    return false;
}

StringRef CleanupPath(StringRef messyPath) {
    auto protocolLocation = strstr(messyPath, "://");

    if(protocolLocation) {
        messyPath = protocolLocation + 3;
    }

#ifdef WIN32
    auto isRelative = ::PathIsRelativeA(messyPath) || messyPath[0] == '/';
#else
    auto isRelative = messyPath[0] != '/';
#endif

    String absolutePath = messyPath;
    if(isRelative) {
        absolutePath = GetCurrentWorkingDirectory();
        absolutePath += "/";
        absolutePath += messyPath;
    }

    std::replace(absolutePath.begin(), absolutePath.end(), '\\', '/');

    Vector<String> pathElements;
    pathElements.clear();
    std::istringstream is(absolutePath);
    String element;
    while(std::getline(is, element, '/')) {
        if(element.length() == 0) {
            // nothing in this element
            continue;
        } else if(element.length() > 1 && element.compare(element.length() - 2, 2, "..") == 0) {
            // parent directory identifier
            pathElements.pop_back();
        } else if(element.compare(element.length() - 1, 1, ".") == 0) {
            // current directory identifier
            continue;
        } else {
            pathElements.push_back(element);
        }
    }

    std::stringstream os;
    os << "file://";

#ifndef WIN32
    os << "/";
#endif

    std::copy(pathElements.begin(), pathElements.end(), std::ostream_iterator<String>(os, "/"));
    auto asStr = os.str();
    asStr.pop_back(); // remove overflowing ending slash from ostream_iterator

    auto result = GetTempString(asStr.c_str());
    return result;
}

static void OnFolderPathChanged(Entity entity, StringRef before, StringRef after) {
    //SetName(entity, GetFileName(after));
}

static void OnFolderAdded(Entity entity) {

}

static void OnFolderRemoved(Entity entity) {

}

static bool ServiceStart() {
    SubscribeFolderAdded(OnFolderAdded);
    SubscribeFolderRemoved(OnFolderRemoved);
    SubscribeFolderPathChanged(OnFolderPathChanged);

    return true;
}

static bool ServiceStop() {
    UnsubscribeFolderAdded(OnFolderAdded);
    UnsubscribeFolderRemoved(OnFolderRemoved);
    UnsubscribeFolderPathChanged(OnFolderPathChanged);

    return true;
}
