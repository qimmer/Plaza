//
// Created by Kim Johannsen on 24/01/2018.
//

#include "CoreModule.h"
#include "Entity.h"

DefineModule(Core)
    ModuleService(Entity)
    ModuleService(Hierarchy)
    ModuleService(FunctionCallService)

    ModuleType(Hierarchy)
EndModule()

void* operator new[](size_t size, const char* pName, int flags, unsigned     debugFlags, const char* file, int line)
{
    return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return malloc(size);
}