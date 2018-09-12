//
// Created by Kim Johannsen on 16/01/2018.
//

#include "CommandLine.h"
#include <Core/NativeUtils.h>

struct CommandLineArgument {
    StringRef CommandLineArgumentName;
    StringRef CommandLineArgumentValue;
};

BeginUnit(CommandLineArgument)
    BeginComponent(CommandLineArgument)
        RegisterProperty(StringRef, CommandLineArgumentName)
        RegisterProperty(StringRef, CommandLineArgumentValue)
    EndComponent()
EndUnit()

void ParseCommandLine(int argc, char** argv) {

}
