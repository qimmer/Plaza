//
// Created by Kim Johannsen on 08/01/2018.
//

#include <Core/NativeUtils.h>
#include <Foundation/FoundationModule.h>
#include <File/Folder.h>
#include <File/FileStream.h>

BeginModule(File)
    RegisterUnit(Folder)
    RegisterUnit(FileStream)

    RegisterDependency(Foundation)
EndModule()
