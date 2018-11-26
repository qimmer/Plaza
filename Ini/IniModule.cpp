//
// Created by Kim on 08-11-2018.
//

#include "IniModule.h"
#include "IniPersistance.h"

#include <Foundation/FoundationModule.h>

BeginModule(Ini)
    RegisterDependency(Foundation)
    RegisterUnit(IniPersistance)
EndModule()
