//
// Created by Kim Johannsen on 16/01/2018.
//

#include "CommandLine.h"
#include <Core/NativeUtils.h>

struct CommandLineArgument {
    char CommandLineArgumentName[32];
    char CommandLineArgumentValue[1024-32];
};

BeginUnit(CommandLineArgument)
    BeginComponent(CommandLineArgument)
        RegisterProperty(StringRef, CommandLineArgumentName)
        RegisterProperty(StringRef, CommandLineArgumentValue)
    EndComponent()
EndUnit()

void ParseCommandLine(int argc, char** argv) {

}
