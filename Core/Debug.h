//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_DEBUG_H
#define PLAZA_DEBUG_H

#include <stdio.h>
#include <Core/NativeUtils.h>

#define Assert(CONTEXT, x) \
    if(!(x)) {\
        Log( CONTEXT, LogSeverity_Fatal, "Assertion failed for %s at line %d in %s: %s\n\n%s", GetDebugName(CONTEXT), __LINE__, __FILE__, #x, GetStackTrace());\
        Exit(-1);\
     } do{} while(false)

#ifdef NDEBUGG
#define Verbose(verboseTag, fmt, ...) if(VerboseTag && strstr(verboseTag, VerboseTag)) { printf(fmt "\n", ##__VA_ARGS__); } do {} while(false)
#else
#define Verbose(verboseLevel, fmt, ...)
#endif

#define LogSeverity_Info 0
#define LogSeverity_Warning 1
#define LogSeverity_Error 2
#define LogSeverity_Fatal 3

void Log(Entity context, int severity, StringRef format, ...);

void Error(Entity context, StringRef format, ...);
void Warning(Entity context, StringRef format, ...);
void Info(Entity context, StringRef format, ...);

void DebuggerBreak();
void Exit(s32 returnCode);

StringRef GetDebugName(Entity entity);
void DumpNode();

StringRef GetStackTrace();

#include <Core/NativeUtils.h>

Unit(Debug)

#endif //PLAZA_DEBUG_H
