//
// Created by Kim Johannsen on 08/01/2018.
//

#include "FoundationModule.h"
#include "Stream.h"
#include "File/FileStream.h"
#include "MemoryStream.h"
#include "File/Folder.h"
#include <Core/Hierarchy.h>
#include <Foundation/Visibility.h>
#include <Core/CoreModule.h>

DefineModule(File)
    ModuleType(Folder)
    ModuleService(FileStream)
    ModuleService(Folder)

    ModuleDependency(Foundation)
EndModule()
