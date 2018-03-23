//
// Created by Kim Johannsen on 07/01/2018.
//

#ifndef PLAZA_TESTFILE_H
#define PLAZA_TESTFILE_H

#include <Test/Test.h>
#include <Core/Entity.h>
#include <Foundation/Stream.h>
#include <File/FileStream.h>
#include <Core/Module.h>
#include <Foundation/FoundationModule.h>
#include "Common.h"

TestResult TestFile()
{
    InitializeModule(ModuleOf_Foundation());

    auto file = CreateStream(0, "testStream");
    SetStreamPath(file, "file://test.bin");

    // Test write
    Assert(StreamOpen(file, StreamMode_Write));
    Assert(StreamWrite(file, 6, "Hello") == 6);
    StreamClose(file);

    // Test read
    Assert(StreamOpen(file, StreamMode_Read));
    Assert(StreamSeek(file, 1));
    char buffer[6];
    Assert(StreamRead(file, 5, buffer) == 5);
    Assert(strcmp(buffer, "ello") == 0);
    StreamClose(file);

    DestroyEntity(file);

    return Success;
}

#endif //PLAZA_TESTFILE_H
