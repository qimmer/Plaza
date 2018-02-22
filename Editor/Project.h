
#ifndef Project_H
#define Project_H

#include <Core/Entity.h>

DeclareComponent(Project)
DeclareService(Project)

DeclareComponentChild(Project, ProjectSourceFolder)
DeclareComponentChild(Project, ProjectBuildFolder)

void ScanProjects();

#endif //Project_H
