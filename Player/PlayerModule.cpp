//
// Created by Kim Johannsen on 17/01/2018.
//

#include <Foundation/FoundationModule.h>
#include <Rendering/RenderingModule.h>
#include <File/FileModule.h>
#include "PlayerModule.h"
#include "PlayerContext.h"

BeginModule(Player)
    ModuleDependency(Foundation)
    ModuleDependency(Rendering)
    ModuleService(PlayerContext)

    ModuleDependency(Foundation)
    ModuleDependency(File)
EndModule()
