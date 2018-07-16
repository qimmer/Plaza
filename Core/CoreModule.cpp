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
#include <Core/Node.h>
#include <Core/Debug.h>

#include <malloc.h>

void __InitializeBase() {
    __InitializeComponent();
    __InitializeProperty();
    __InitializeNode();
    __InitializeEvent();
    __InitializeFunction();
}

void __InitModule_Core(Entity module);

API_EXPORT Entity ModuleOf_Core () {
    static Entity module = 0;
    static bool firstTime = false;
    if(!module) {
        module = GetUniqueEntity("Module Core", &firstTime);
        if(firstTime) {
            ComponentOf_Node();
            PropertyOf_Name();
            SetParent(module, GetModuleRoot());
            SetName(module, "Core");
            __InitModule_Core(module);

            SetModuleSourcePath(module, __FILE__);
            SetModuleVersion(module, __DATE__ " " __TIME__);
        }
    }
    return module;
}

void __InitModule_Core (Entity module) {
    __InitializeBase();
    RegisterUnit(Property)
    RegisterUnit(Event)
    RegisterUnit(Function)
    RegisterUnit(Entity)
    RegisterUnit(Component)
    RegisterUnit(Module)
    RegisterUnit(Node)
    RegisterUnit(Debug)
EndModule()

API_EXPORT void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return ::malloc(size);
}

API_EXPORT void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return ::malloc(size);
}