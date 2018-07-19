//
// Created by Kim on 28/04/2018.
//

#include <Foundation/PersistancePoint.h>
#include <Foundation/Stream.h>
#include <Foundation/VirtualPath.h>
#include "CppPersistance.h"
#include "ModuleDefinition.h"

#include <sstream>

DefineService(CppPersistance)
EndService()

static void GenerateComponent(Entity componentDefinition) {

}

static void WriteModuleHeader(Entity moduleDefinition, std::ostream& moduleHeaderCode) {
    moduleHeaderCode << "#ifndef " << GetName(moduleDefinition) << "Module_H" << std::endl;
    moduleHeaderCode << "#define " << GetName(moduleDefinition) << "Module_H" << std::endl;
    moduleHeaderCode << "" << std::endl;
    moduleHeaderCode << "#include <Core/Module.h>" << std::endl;
    moduleHeaderCode << "" << std::endl;
    moduleHeaderCode << "Module(" << GetName(moduleDefinition) << ")" << std::endl;
    moduleHeaderCode << "" << std::endl;
    moduleHeaderCode << "#endif" << std::endl;
}

static void WriteModuleSource(Entity moduleDefinition, std::ostream& moduleSourceCode) {
    moduleSourceCode << "<Core/Module.h>" << std::endl;

    for_children(componentDependency, GetModuleDefinitionDependencies(moduleDefinition)) {
        auto dependencyName = GetName(GetModuleDependencyModuleDefinition(componentDependency));
        moduleSourceCode << "#include <" << dependencyName << "/" << dependencyName << "Module.h>" << std::endl;
    }

    moduleSourceCode << "" << std::endl;
    moduleSourceCode << "BeginModule(" << GetName(moduleDefinition) << ")" << std::endl;

    for_children(serviceDefinition, GetModuleDefinitionComponents(moduleDefinition)) {
        moduleSourceCode << "    ModuleService(" << GetName(serviceDefinition) << ")" << std::endl;
    }

    moduleSourceCode << "" << std::endl;

    for_children(componentDefinition, GetModuleDefinitionServices(moduleDefinition)) {
        moduleSourceCode << "    ModuleType(" << GetName(componentDefinition) << ")" << std::endl;
    }

    moduleSourceCode << "EndModule()" << std::endl;
}

static void WriteComponentHeader(Entity componentDefinition, std::ostream& code) {

}

API_EXPORT void GenerateModuleSources(Entity moduleDefinition) {
    char moduleVirtualPath[PathMax];
    char moduleResolvedPath[PathMax];

    snprintf(moduleVirtualPath, PathMax, "file://%s/../../%s", __FILE__, GetName(moduleDefinition));
    ResolveVirtualPath(moduleVirtualPath, moduleResolvedPath);

    char moduleSourceFilePath[PathMax];
    char moduleHeaderFilePath[PathMax];

    snprintf(moduleHeaderFilePath, PathMax, "%s/%sModule.h", moduleResolvedPath, GetName(moduleDefinition));
    snprintf(moduleSourceFilePath, PathMax, "%s/%sModule.cpp", moduleResolvedPath, GetName(moduleDefinition));

    std::stringstream moduleHeaderCode, moduleSourceCode;
    WriteModuleHeader(moduleDefinition, moduleHeaderCode);
    WriteModuleSource(moduleDefinition, moduleSourceCode);

    for_children(componentDefinition, GetModuleDefinitionComponents(moduleDefinition)) {
        char componentSourceFilePath[PathMax];
        char componentHeaderFilePath[PathMax];

        std::stringstream headerCode, sourceCode;

        snprintf(moduleHeaderFilePath, PathMax, "%s/%sModule.h", moduleResolvedPath, GetName(moduleDefinition));
        snprintf(moduleSourceFilePath, PathMax, "%s/%sModule.cpp", moduleResolvedPath, GetName(moduleDefinition));

        auto headerStream = CreateStream(componentDefinition, "HeaderStream");
        SetStreamPath(headerStream, componentHeaderFilePath);

        auto sourceStream = CreateStream(componentDefinition, "SourceStream");
        SetStreamPath(sourceStream, componentSourceFilePath);
    }
}
