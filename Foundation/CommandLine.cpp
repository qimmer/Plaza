//
// Created by Kim Johannsen on 16/01/2018.
//

#include "CommandLine.h"

struct CommandLine {
    String CommandLineArgumentName, CommandLineArgumentValue;
};

DefineComponent(CommandLineArgument)
EndComponent()

DefineComponentProperty(CommandLineArgument, StringRef, CommandLineArgumentName)
DefineComponentProperty(CommandLineArgument, StringRef, CommandLineArgumentValue)

void ParseCommandLine(int argc, char** argv) {
    char commandLine[PATH_MAX];
    memset(commandLine, 0, PATH_MAX);

    for(auto i = 0; i < argc; ++i) {
        strcat(commandLine, argv[i]);
        strcat(commandLine, " ");
    }
    strstr(commandLine, )

}
