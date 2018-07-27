//
// Created by Kim on 27-07-2018.
//

#include <Foundation/NativeUtils.h>
#include <Foundation/FoundationModule.h>
#include <Networking/NetworkingModule.h>
#include <Json/JsonModule.h>
#include <Rest/RestModule.h>
#include <File/FileModule.h>
#include "DebugModule.h"

BeginUnit(Debug)
    RegisterFileType(".html", "text/html", 0)
    RegisterFileType(".css", "text/css", 0)
    RegisterFileType(".js", "application/javascript", 0)
EndUnit()

BeginModule(Debug)
    RegisterDependency(Core)
    RegisterDependency(Foundation)
    RegisterDependency(Networking)
    RegisterDependency(Json)
    RegisterDependency(Rest)
    RegisterDependency(File)
    RegisterUnit(Debug)
    RegisterData("file://Debug.json")
EndModule()