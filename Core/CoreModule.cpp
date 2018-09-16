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

#include <malloc.h>

bool __IsCoreInitialized = false;

void __InitializeBase() {
    SetComponentSize(ComponentOf_Component(), 500);
    SetComponentSize(ComponentOf_Property(), 38);
    SetComponentSize(ComponentOf_Ownership(), 8);
    SetComponentSize(ComponentOf_Identification(), 1024);
    SetComponentSize(ComponentOf_Event(), 1000);
    SetComponentSize(ComponentOf_EventArgument(), 5);
    SetComponentSize(ComponentOf_Subscription(), 24);
    SetComponentSize(ComponentOf_Function(), 250);
    SetComponentSize(ComponentOf_FunctionArgument(), 8);
    SetComponentSize(ComponentOf_Module(), 8196);
    SetComponentSize(ComponentOf_ModuleRoot(), 720);
    SetComponentSize(ComponentOf_Enum(), 4);
    SetComponentSize(ComponentOf_EnumFlag(), 4);

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
        module = GetUniqueEntity("Module Core", &firstTime);
        __InitModule_Core(module);
    }
    return module;
}

void __InitModule_Core (Entity module) {
    __InitializeBase();
    AddComponent(module, ComponentOf_Module());
    RegisterUnit(Module)
    RegisterUnit(Component)
    RegisterUnit(Enum)
    RegisterUnit(Property)
    RegisterUnit(Event)
    RegisterUnit(Function)
    RegisterUnit(Entity)
    RegisterUnit(Date)
    RegisterUnit(Core)
    RegisterUnit(Debug)
    RegisterUnit(Identification)

    SetName(GetModuleRoot(), "Modules");
    AddComponent(GetModuleRoot(), ComponentOf_ModuleRoot());

    __InjectArrayPropertyElement(PropertyOf_Modules(), GetModuleRoot(), module);
    SetName(module, "Core");

    SetModuleSourcePath(module, __FILE__);
    SetModuleVersion(module, __DATE__ " " __TIME__);
EndModule()

struct CoreModule {
    Vector(LogMessages, Entity, 1024)
};

BeginUnit(Core)
    BeginComponent(CoreModule)
        RegisterArrayProperty(LogMessage, LogMessages)
    EndComponent()
EndUnit()