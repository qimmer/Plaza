//
// Created by Kim Johannsen on 04-04-2018.
//

#include <Scene/SceneModule.h>
#include "UIModule.h"
#include "Font.h"
#include "Label.h"
#include "Widget.h"
#include "UICamera.h"

BeginModule(UI)
    ModuleDependency(Scene)

    ModuleService(Label)
    ModuleService(Font)
    ModuleService(Widget)
    ModuleService(UICamera)

    ModuleType(Font)
    ModuleType(Label)
    ModuleType(Widget)
    ModuleType(UICamera)
EndModule()
