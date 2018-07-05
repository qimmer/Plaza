//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_DEBUG_H
#define PLAZA_DEBUG_H

#include <Core/NativeUtils.h>
#include <stdio.h>

#define Assert(CONTEXT, x) if(!(x)) { Log( CONTEXT, LogSeverity_Fatal, "Assertion failed at line %d in %s: %s", __LINE__, __FILE__, #x ); Exit(-1); } do{} while(false)

#ifdef VERBOSE
#define Verbose(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define Verbose(fmt, ...)
#endif

#define LogSeverity_Info 0
#define LogSeverity_Warning 1
#define LogSeverity_Error 2
#define LogSeverity_Fatal 3

void Log(Entity context, int severity, StringRef format, ...);
void DebuggerBreak();
void Exit(s32 returnCode);

struct LogMessageReceivedArgs
{
    Entity Entity;
    char LogMessageText[1024 - 64 - 1];
    u8 LogMessageSeverity;
};

Unit(Debug)
    Event(LogMessageReceived)

#endif //PLAZA_DEBUG_H
