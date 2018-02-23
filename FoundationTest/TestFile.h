//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_TESTFILE_H
#define PLAZA_TESTFILE_H

#include <Test/Test.h>
#include <Core/Entity.h>
#include <File/FileStream.h>
#include <Core/Module.h>
#include <Foundation/FoundationModule.h>
#include "Common.h"

using namespace Plaza;

TestResult TestFile()
{
    InitializeModule(ModuleOf_Foundation());

    auto file = CreateEntity();
    AddFileStream(file);

    SetFilePath(file, "test.bin");

    // Test write
    SetFileMode(file, FileMode_Write);
    Assert(StreamOpen(file));
    Assert(StreamWrite(file, 6, "Hello") == 6);
    StreamClose(file);

    // Test read
    SetFileMode(file, FileMode_Write);
    Assert(StreamOpen(file));
    Assert(StreamSeek(file, 1));
    char buffer[6];
    Assert(StreamRead(file, 5, buffer) == 5);
    Assert(strcmp(buffer, "ello") == 0);
    StreamClose(file);

    DestroyEntity(file);

    return Success;
}

#endif //PLAZA_TESTFILE_H
