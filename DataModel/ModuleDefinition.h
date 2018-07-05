//
// Created by Kim on 22-04-2018.
//

#ifndef PLAZA_MODULEDEFINITION_H
#define PLAZA_MODULEDEFINITION_H

#include <Core/Entity.h>

DeclareComponent(ModuleDefinition)
    DeclareComponentChild(ModuleDefinition, ModuleDefinitionDependencies)
    DeclareComponentChild(ModuleDefinition, ModuleDefinitionComponents)
    DeclareComponentChild(ModuleDefinition, ModuleDefinitionServices)
    DeclareComponentChild(ModuleDefinition, ModuleDefinitionEnums)

DeclareComponent(ModuleDependencyDefinition)
    DeclareComponentPropertyReactive(ModuleDependencyDefinition, Entity, ModuleDependencyModuleDefinition)

#endif //PLAZA_MODULEDEFINITION_H
