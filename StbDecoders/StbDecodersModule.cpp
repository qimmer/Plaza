//
// Created by Kim Johannsen on 06-03-2018.
//

#include <Foundation/FoundationModule.h>
#include "StbDecodersModule.h"
#include "StbImagePersistance.h"

DefineModule(StbDecoders)
        ModuleDependency(Foundation)

        ModuleService(StbImagePersistance)
EndModule()
