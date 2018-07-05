//
// Created by Kim Johannsen on 24/01/2018.
//

#include "CoreModule.h"
#include <Core/Types.h>
#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Property.h>
#include <Core/Event.h>
#include <Core/Module.h>
#include <Core/Hierarchy.h>
#include <Core/Debug.h>

#include <malloc.h>

void __InitializeBase() {
    __InitializeComponent();
    __InitializeProperty();
    __InitializeHierarchy();
    __InitializeEvent();
    __InitializeFunction();
}

BeginModule(Core)
    __InitializeBase();
    RegisterUnit(Property)
    RegisterUnit(Function)
    RegisterUnit(Event)
    RegisterUnit(Component)
    RegisterUnit(Entity)
    RegisterUnit(Module)
    RegisterUnit(Hierarchy)
    RegisterUnit(Debug)
EndModule()

API_EXPORT void* operator new[](size_t size, const char* pName, int flags, unsigned     debugFlags, const char* file, int line)
{
    return ::malloc(size);
}

API_EXPORT void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return ::malloc(size);
}