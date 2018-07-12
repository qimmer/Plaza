//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_COMMANDLINE_H
#define PLAZA_COMMANDLINE_H

#include <Core/NativeUtils.h>

Unit(CommandLineArgument)
    Component(CommandLineArgument)
        Property(StringRef, CommandLineArgumentName)
        Property(StringRef, CommandLineArgumentValue)

void ParseCommandLine(int argc, char** argv);

#endif //PLAZA_COMMANDLINE_H
