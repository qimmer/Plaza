//
// Created by Kim Johannsen on 16/01/2018.
//

#ifndef PLAZA_COMMANDLINE_H
#define PLAZA_COMMANDLINE_H

#include <Core/Entity.h>
#include <Core/Service.h>

DeclareComponent(CommandLineArgument)

DeclareComponentProperty(CommandLineArgument, StringRef, CommandLineArgumentName)
DeclareComponentProperty(CommandLineArgument, StringRef, CommandLineArgumentValue)

void ParseCommandLine(int argc, char** argv);

#endif //PLAZA_COMMANDLINE_H
