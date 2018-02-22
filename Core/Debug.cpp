//
// Created by Kim Johannsen on 21/12/2017.
//

#include <Core/Debug.h>

#include <stdio.h>
#include <stdarg.h>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <signal.h>
#endif

DefineEvent(LogMessageReceived, LogHandler)

void Log(int channel, int severity, StringRef format, ...) {
    char buffer[4096];
    va_list arg;

    switch (severity) {
        case LogSeverity_Info:
            sprintf(buffer, "Info:    ");
            break;
        case LogSeverity_Warning:
            sprintf(buffer, "Warning: ");
            break;
        case LogSeverity_Error:
            sprintf(buffer, "Error:   ");
            DebuggerBreak();
            break;
        case LogSeverity_Fatal:
            sprintf(buffer, "Fatal:   ");
            DebuggerBreak();
            break;
    }

    va_start(arg, format);
    auto numWritten = vsnprintf(buffer + 9, 4084, format, arg);
    va_end(arg);

    buffer[numWritten + 9] = '\n';
    buffer[numWritten + 10] = '\0';

    printf("%s", buffer);

    FireEvent(LogMessageReceived, channel, severity, buffer);
}

void DebuggerBreak() {
#ifdef _MSC_VER
    __debugbreak()
#else
    raise(SIGTRAP);
#endif
}
