//
// Created by Kim Johannsen on 23/01/2018.
//

#include <dirent.h>
#include <unistd.h>
#include <Core/String.h>
#include <climits>
#include "Folder.h"
#include "Core/Hierarchy.h"
#include "FileStream.h"


    struct Folder {
        String FolderPath;
    };

    DefineComponent(Folder)
        Dependency(Hierarchy)
    EndComponent()

    DefineService(Folder)
        ServiceDependency(FileStream)
    EndService()

    DefineComponentProperty(Folder, StringRef, FolderPath)

    StringRef GetCurrentWorkingDirectory() {
        static char path[PATH_MAX];
        getcwd(path, PATH_MAX);
        return path;
    }

    StringRef GetFileName(StringRef absolutePath) {
        auto fileName = strrchr(absolutePath, GetPathSeparator());
        if(!fileName) return absolutePath;
        return fileName + 1;
    }

    StringRef GetFileExtension(StringRef absolutePath) {
        auto extension = strrchr(absolutePath, '.');
        if(!extension) return "";
        return extension;
    }

    char GetPathSeparator() {
#ifdef WIN32
        return '\\';
#endif
        return '/';
    }

    StringRef GetParentFolder(StringRef absolutePath) {
        static char parentFolder[PATH_MAX];
        strcpy(parentFolder, absolutePath);
        auto ptr = strrchr(parentFolder, GetPathSeparator());
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

                if(ent->d_type == DT_DIR) {
                    entryEntity = CreateFolder(entityPath);
                    SetName(entryEntity, ent->d_name);
                    SetParent(entryEntity, entity);
                    SetFolderPath(entryEntity, entryPath);
                } else if(ent->d_type == DT_REG) {
                    entryEntity = CreateFileStream(entityPath);
                    SetName(entryEntity, ent->d_name);
                    SetParent(entryEntity, entity);
                    SetFilePath(entryEntity, entryPath);
                }
            }
            closedir (dir);
        }
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
