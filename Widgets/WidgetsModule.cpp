//
// Created by Kim on 17-01-2019.
//

#include "WidgetsModule.h"
#include <Gui/GuiModule.h>
#include <Json/NativeUtils.h>

#include "Button.h"
#include "Checkbox.h"
#include "Dock.h"
#include "MainFrame.h"
#include "Panel.h"
#include "Style.h"
#include "Menu.h"
#include "ListItem.h"

BeginModule(Widgets)
    RegisterDependency(Gui)

    RegisterUnit(Style)
    RegisterUnit(Button)
    //RegisterUnit(Checkbox)
    RegisterUnit(Dock)
    RegisterUnit(Panel)
    RegisterUnit(Menu)
    RegisterUnit(ListItem)

    RegisterUnit(MainFrame)
EndModule()