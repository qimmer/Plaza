//
// Created by Kim Johannsen on 19-03-2018.
//

#include "InputModule.h"
#include "InputContext.h"
#include "Key.h"

BeginModule(Input)
    RegisterUnit(InputContext)
    RegisterUnit(Key)
EndModule()