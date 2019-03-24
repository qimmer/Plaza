//
// Created by Kim Johannsen on 24/01/2018.
//

#include "CoreModule.h"
#include "Identification.h"
#include "Enum.h"
#include <Core/Types.h>
#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Property.h>
#include <Core/Event.h>
#include <Core/Module.h>
#include <Core/Date.h>
#include <Core/Debug.h>
#include <Core/Instance.h>
#include <Core/Binding.h>

#include <malloc.h>

bool __IsCoreInitialized = false;

void __InitializeBase() {
    __InitializeComponent();
    __InitializeModule();
    __InitializeProperty();
    __InitializeEvent();
    __InitializeFunction();
}

void __InitModule_Core(Entity module);

API_EXPORT Entity ModuleOf_Core () {
    static Entity module = 0;
    static bool firstTime = false;
    if(!module) {
        module = GetUniqueEntity("Module.Core", &firstTime);
        __InitModule_Core(module);
    }
    return module;
}

void __InitModule_Core (Entity module) {
    __PreInitialize();
    __InitializeBase();
    AddComponent(module, ComponentOf_Module());
	SetUuid(module, "Module.Core");

	RegisterUnit(Entity)
	RegisterUnit(Component)
	RegisterUnit(Enum)
	RegisterUnit(Property)
	RegisterUnit(Function)
	RegisterUnit(Event)
    RegisterUnit(Module)
    RegisterUnit(Date)
    RegisterUnit(Debug)
    RegisterUnit(Identification)
    RegisterUnit(Binding)
    RegisterUnit(Instance)

    AddComponent(GetModuleRoot(), ComponentOf_ModuleRoot());
	SetUuid(GetModuleRoot(), "ModuleRoot");

    AddChild(PropertyOf_Modules(), GetModuleRoot(), module, true);
    SetOwner(module, GetModuleRoot(), PropertyOf_Modules());

    SetModuleSourcePath(module, __FILE__);
    SetModuleVersion(module, __DATE__ " " __TIME__);
EndModule()
