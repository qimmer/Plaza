//
// Created by Kim on 22-04-2018.
//

#include "ModuleDefinition.h"

struct ModuleDefinition {
    Entity ModuleDefinitionDependencies,
            ModuleDefinitionComponents,
            ModuleDefinitionServices,
            ModuleDefinitionEnums;
};

struct ModuleDependencyDefinition {
    Entity ModuleDependencyModuleDefinition;
};

DefineComponent(ModuleDefinition)
EndComponent()
DefineComponentChild(ModuleDefinition, Hierarchy, ModuleDefinitionDependencies)
DefineComponentChild(ModuleDefinition, Hierarchy, ModuleDefinitionComponents)
DefineComponentChild(ModuleDefinition, Hierarchy, ModuleDefinitionServices)


DefineComponent(ModuleDependencyDefinition)
    DefineProperty(Entity, ModuleDependencyModuleDefinition)
EndComponent()
DefineComponentPropertyReactive(ModuleDependencyDefinition, Entity, ModuleDependencyModuleDefinition)
