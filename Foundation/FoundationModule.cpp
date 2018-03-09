//
// Created by Kim Johannsen on 08/01/2018.
//

#include "FoundationModule.h"
#include "Stream.h"
#include "MemoryStream.h"
#include "File/Folder.h"
#include "Persistance.h"
#include "PersistancePoint.h"
#include "PropertyBinding.h"
#include <Core/Hierarchy.h>
#include <Foundation/Visibility.h>
#include <Core/CoreModule.h>

DefineModule(Foundation)
    ModuleType(Visibility)
    ModuleType(Stream)
    ModuleType(Persistance)
    ModuleType(PersistancePoint)
    ModuleType(PropertyBinding)

    ModuleService(MemoryStream)
    ModuleService(Hierarchy)
    ModuleService(Stream)

    ModuleDependency(Core)
EndModule()
