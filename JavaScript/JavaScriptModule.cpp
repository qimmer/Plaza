//
// Created by Kim on 10/07/2018.
//

#include <Core/NativeUtils.h>
#include "JavaScriptModule.h"
#include <JavaScript/JavaScriptPersistance.h>

BeginModule(JavaScript)
    RegisterUnit(JavaScriptContext)
    RegisterUnit(JavaScriptPersistance)
EndModule()