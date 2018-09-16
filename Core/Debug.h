//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_DEBUG_H
#define PLAZA_DEBUG_H

#include <stdio.h>
#include <Core/NativeUtils.h>

#define VerboseLevel_Invocations 0
#define VerboseLevel_PropertyChanges 1
#define VerboseLevel_ComponentEntityCreationDeletion 2
#define VerboseLevel_Common 3

#define VerboseLevel 4

#define Assert(CONTEXT, x) \
    if(!(x)) {\
        Log( CONTEXT, LogSeverity_Fatal, "Assertion failed for %s at line %d in %s: %s\n\n%s", GetDebugName(CONTEXT), __LINE__, __FILE__, #x, GetStackTrace());\
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

StringRef GetStackTrace();

#include <Core/NativeUtils.h>

Unit(Debug)
    Component(LogMessage)
        Property(StringRef, LogMessageEntity)
        Property(StringRef, LogMessageText)
        Property(StringRef, LogMessageStackTrace)
        Property(u8, LogMessageSeverity)
        Property(u32, LogMessageCount)
        Property(Date, LogMessageTime)

#endif //PLAZA_DEBUG_H
