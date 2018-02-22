
#include "ProjectReference.h"

struct ProjectReference {
    Entity ProjectReferenceProject;
};

DefineComponent(ProjectReference)
EndComponent()

DeclareComponentProperty(ProjectReference, Entity, ProjectReferenceProject)