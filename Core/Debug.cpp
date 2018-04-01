//
// Created by Kim Johannsen on 21/12/2017.
//

#include <Core/Debug.h>
#include <Core/Delegate.h>

#include <stdio.h>
#include <stdarg.h>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <signal.h>
#endif

DeclareEvent(LogMessageReceived, LogHandler)

DefineEvent(LogMessageReceived, LogHandler)

void Log(int channel, int severity, StringRef format, ...) {
    char buffer[4096];
    va_list arg;

    switch (severity) {
        case LogSeverity_Info:
            snprintf(buffer, 4096, "Info:    ");
            break;
        case LogSeverity_Warning:
            snprintf(buffer, 4096, "Warning: ");
            break;
        case LogSeverity_Error:
            snprintf(buffer, 4096, "Error:   ");
            break;
        case LogSeverity_Fatal:
            snprintf(buffer, 4096, "Fatal:   ");
            break;
    }

    va_start(arg, format);
    auto numWritten = vsnprintf(buffer + 9, 4084, format, arg);
    va_end(arg);

    buffer[numWritten + 9] = '\n';
    buffer[numWritten + 10] = '\0';

    printf("%s", buffer);

    FireEvent(LogMessageReceived, channel, severity, buffer);

    if(severity > LogSeverity_Error) {
        DebuggerBreak();
    }
}

void DebuggerBreak() {
#ifdef WIN32
    __debugbreak();
#else
    raise(SIGTRAP);
#endif
}
