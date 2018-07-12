//
// Created by Kim Johannsen on 20/02/2018.
//

#include <SQL/SQLModule.h>
#include <Foundation/FoundationModule.h>
#include "SQLPersistancePoint.h"


BeginModule(SQL)
    ModuleDependency(Foundation)

    ModuleService(SQLPersistancePoint)
    ModuleType(SQLPersistancePoint)
EndModule()
