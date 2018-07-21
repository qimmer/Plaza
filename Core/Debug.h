//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_DEBUG_H
#define PLAZA_DEBUG_H

#include <stdio.h>
#include <Core/Types.h>

#define VerboseLevel_Invocations 0
#define VerboseLevel_PropertyChanges 1
#define VerboseLevel_ComponentEntityCreationDeletion 2
#define VerboseLevel_Common 3

#define VerboseLevel 1

#define Assert(CONTEXT, x) \
    if(!(x)) {\
        Log( CONTEXT, LogSeverity_Fatal, "Assertion failed for %s at line %d in %s: %s", GetDebugName(CONTEXT), __LINE__, __FILE__, #x );\
        Exit(-1);\
     } do{} while(false)

#if VerboseLevel
#define Verbose(verboseLevel, fmt, ...) if(verboseLevel >= VerboseLevel) { printf(fmt "\n", ##__VA_ARGS__); } do {} while(false)
#else
#define Verbose(verboseLevel, fmt, ...)
#endif

#define LogSeverity_Info 0
#define LogSeverity_Warning 1
#define LogSeverity_Error 2
#define LogSeverity_Fatal 3

void Log(Entity context, int severity, StringRef format, ...);
void DebuggerBreak();
void Exit(s32 returnCode);

StringRef GetDebugName(Entity entity);
void DumpNode();

#include <Core/NativeUtils.h>

Unit(Debug)
    Event(LogMessageReceived, StringRef message, u8 severity)

#endif //PLAZA_DEBUG_H
