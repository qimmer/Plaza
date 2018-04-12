//
// Created by Kim Johannsen on 04-04-2018.
//

#include <Scene/SceneModule.h>
#include "UIModule.h"
#include "Font.h"
#include "Label.h"

DefineModule(UI)
    ModuleDependency(Scene)

    ModuleService(Label)
    ModuleService(Font)

    ModuleType(Font)
    ModuleType(Label)
EndModule()
