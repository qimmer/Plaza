//
// Created by Kim Johannsen on 08/01/2018.
//

#include "FoundationModule.h"
#include "Stream.h"
#include "File/FileStream.h"
#include "Invalidation.h"
#include "MemoryStream.h"
#include "File/Folder.h"
#include <Core/Hierarchy.h>
#include <Foundation/Visibility.h>
#include <Core/CoreModule.h>

DefineModule(Foundation)
    ModuleType(Hierarchy)
    ModuleType(Visibility)
    ModuleType(Stream)
    ModuleType(Invalidation)

    ModuleService(MemoryStream)
    ModuleService(Hierarchy)
    ModuleService(Invalidation)

    ModuleDependency(Core)
EndModule()
