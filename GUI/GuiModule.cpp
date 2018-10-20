//
// Created by Kim on 19-10-2018.
//

#include <Json/NativeUtils.h>
#include <Foundation/FoundationModule.h>
#include <Rendering/RenderingModule.h>
#include <Scene/SceneModule.h>

#include "GuiModule.h"
#include "Widget.h"
#include "Font.h"

BeginModule(Gui)
    RegisterDependency(Foundation)
    RegisterDependency(Rendering)
    RegisterDependency(Scene)

    RegisterUnit(Widget)
    RegisterUnit(Font)
EndModule()
