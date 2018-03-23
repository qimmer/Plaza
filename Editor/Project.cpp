
#include <File/Folder.h>
#include <Core/String.h>
#include <Core/Module.h>
#include <Foundation/PersistancePoint.h>
#include "Project.h"

struct Project {
    Entity ProjectSourceFolder, ProjectBuildFolder;
};

DefineComponent(Project)
        Dependency(PersistancePoint)
EndComponent()

DefineService(Project)
EndService()

DefineComponentChild(Project, Folder, ProjectSourceFolder)
DefineComponentChild(Project, Folder, ProjectBuildFolder)

void ScanProjects() {
    Entity projectRoot = CreateHierarchy(0, "Projects");
    for(auto module = GetNextModule(0); IsModuleValid(module); module = GetNextModule(module)) {
        auto sourceFilePath = GetModuleSourcePath(module);

        char projectFolder[PATH_MAX];
        GetParentFolder(sourceFilePath, projectFolder, PATH_MAX);

        auto project = CreateProject(projectRoot, GetModuleName(module));
        SetFolderPath(GetProjectSourceFolder(project), projectFolder);
        ScanFolder(GetProjectSourceFolder(project));
    }
}

static bool ServiceStart() {
    ScanProjects();
    return true;
}

static bool ServiceStop() {
    return true;
}
