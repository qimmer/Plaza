//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_DEBUG_H
#define PLAZA_DEBUG_H

#include <Core/Types.h>

#define Assert(x) if(!(x)) { Log( LogChannel_Core, LogSeverity_Fatal, "Assertion failed at line %d in %s: %s", __LINE__, __FILE__, #x ); DebuggerBreak(); exit(-1); } do{} while(false)

#define LogChannel_Core 0

#define LogSeverity_Info 0
#define LogSeverity_Warning 1
#define LogSeverity_Error 2
#define LogSeverity_Fatal 3

typedef void(*LogHandler)(int channel, int severity, StringRef format);

void Log(int channel, int severity, StringRef format, ...);
void DebuggerBreak();

#endif //PLAZA_DEBUG_H
