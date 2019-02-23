//
// Created by Kim Johannsen on 21/12/2017.
//

#include <Core/Debug.h>
#include <Core/NativeUtils.h>
#include <Core/Event.h>

#include <stdio.h>
#include <stdarg.h>
#include <cstring>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <signal.h>
#include "Identification.h"

#endif

#ifdef WIN32
#include <process.h>
#undef Enum
#include <Windows.h>
#include <dbghelp.h>
#include "Identification.h"
#include "CoreModule.h"
#include "Debug.h"

#include <Core/Date.h>

#endif

#include <EASTL/map.h>
#include <EASTL/fixed_string.h>


API_EXPORT const char* VerboseTag = NULL;

struct LogMessage {
    StringRef LogMessageEntity;
    StringRef LogMessageText;
    StringRef LogMessageStackTrace;
    u8 LogMessageSeverity;
    u32 LogMessageCount;
    Date LogMessageTime;
};

static StringRef lastLogMessage = 0;

API_EXPORT void Log(Entity context, int severity, StringRef format, va_list arg) {
    char buffer[4096];

    auto numWritten = 0;

    numWritten += vsnprintf(buffer + numWritten, 4095, format, arg);

    buffer[numWritten] = '\0';

    auto interned = Intern(buffer);

    if(lastLogMessage == interned) return; // Prevents log spam

    lastLogMessage = interned;

#ifndef NDEBUG
    setbuf(stdout, 0);
    printf("%s\n", interned);
#endif

    if(severity >= LogSeverity_Error) {
        DebuggerBreak();
    }
}

API_EXPORT void Log(Entity context, int severity, StringRef format, ...) {
    va_list arg;
    va_start(arg, format);
    Log(context, severity, format, arg);
    va_end(arg);
}

API_EXPORT void Error(Entity context, StringRef format, ...) {
    va_list arg;
    va_start(arg, format);
    Log(context, LogSeverity_Error, format, arg);
    va_end(arg);
}

API_EXPORT void Warning(Entity context, StringRef format, ...) {
    va_list arg;
    va_start(arg, format);
    Log(context, LogSeverity_Warning, format, arg);
    va_end(arg);
}

API_EXPORT void Info(Entity context, StringRef format, ...) {
    va_list arg;
    va_start(arg, format);
    Log(context, LogSeverity_Info, format, arg);
    va_end(arg);
}

API_EXPORT void DebuggerBreak() {
#ifdef WIN32
    if(IsDebuggerPresent()) {
        __debugbreak();
    }
#else
    raise(SIGTRAP);
#endif
}

API_EXPORT void Exit(s32 returnCode) {
    ::exit(returnCode);
}

API_EXPORT StringRef GetDebugName(Entity entity) {
    typedef char Path[PathMax];
    static Path paths[8];
    static int currentPath = 0;

    currentPath = (currentPath + 1) % 8;

    if(!IsEntityValid(entity)) return "<Invalid>";

    auto name = GetName(entity);

    if(!name || name[0] == '\0') {
        name = GetUniqueEntityName(entity);
    }

    if(!name || name[0] == '\0') {
        snprintf(paths[currentPath], PathMax, "%llu", entity);
        return paths[currentPath];
    }

    sprintf(paths[currentPath], "%s (%llu)", name, entity);

    return paths[currentPath];
}

static void PrintNode(int level, Entity entity) {
    static const int identation = 4;

    if(!IsEntityValid(entity)) {
        printf("<Invalid>\n");
        return;
    }

    printf("{ (%s) (", GetDebugName(entity));

    for_entity(component2, componentData2, Component) {
        if (!HasComponent(entity, component2)) continue;
        printf("%s, ", GetName(component2));
    }

    printf(")\n");

    for_entity(component, componentData, Component) {
        if(!HasComponent(entity, component)) continue;

        u32 numProperties = 0;
        auto properties = GetProperties(component, &numProperties);
        for(auto i = 0; i < numProperties; ++i) {
            auto property = properties[i];
            auto value = GetPropertyValue(property, entity);
            switch(GetPropertyKind(property)) {
                case PropertyKind_Child:
                    printf("%*s%s: ", (level + 1) * identation, " ", GetDebugName(property));
                    PrintNode(level+1, value.as_Entity);
                    break;
                case PropertyKind_Array:
                    printf("%*s%s: [\n", (level + 1) * identation, " ", GetDebugName(property));
                    u32 count = 0;
                    auto elements = GetArrayPropertyElements(property, entity, &count);
                    for(auto j = 0; j < count; ++j) {
                        printf("%*s [%d]: ", (level + 2) * identation, " ", j);
                        PrintNode(level+2, elements[j]);
                    }
                    printf("%*s]\n", (level + 1) * identation, " ");
                    break;
            }

        }
    }

    printf("%*s}\n", level * identation, " ");
}

API_EXPORT void DumpNode() {
    PrintNode(0, GetModuleRoot());
}

BeginUnit(Debug)
EndUnit()

API_EXPORT StringRef GetStackTrace() {
    eastl::fixed_string<char, 2048> stackTrace;

#ifdef WIN32
    // Set up the symbol options so that we can gather information from the current
    // executable's PDB files, as well as the Microsoft symbol servers.  We also want
    // to undecorate the symbol names we're returned.  If you want, you can add other
    // symbol servers or paths via a semi-colon separated list in SymInitialized.
    ::SymSetOptions( SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME | SYMOPT_OMAP_FIND_NEAREST |SYMOPT_EXACT_SYMBOLS );
    if (!::SymInitialize( ::GetCurrentProcess(), "http://msdl.microsoft.com/download/symbols", TRUE )) return Intern("");

    // Capture up to 25 stack frames from the current call stack.  We're going to
    // skip the first stack frame returned because that's the GetStackWalk function
    // itself, which we don't care about.
    PVOID addrs[ 62 ] = { 0 };
    USHORT frames = CaptureStackBackTrace( 1, 62, addrs, NULL );

    for (USHORT i = 0; i < frames; i++) {
        // Allocate a buffer large enough to hold the symbol information on the stack and get
        // a pointer to the buffer.  We also have to set the size of the symbol structure itself
        // and the number of bytes reserved for the name.
        ULONG64 buffer[ (sizeof( SYMBOL_INFO ) + 1024 + sizeof( ULONG64 ) - 1) / sizeof( ULONG64 ) ] = { 0 };
        SYMBOL_INFO *info = (SYMBOL_INFO *)buffer;
        info->SizeOfStruct = sizeof( SYMBOL_INFO );
        info->MaxNameLen = 1024;

        // Attempt to get information about the symbol and add it to our output parameter.
        DWORD64 displacement = 0;
        if (::SymFromAddr( ::GetCurrentProcess(), (DWORD64)addrs[ i ], &displacement, info )) {
            char unmangledName[512];
            UnDecorateSymbolName(info->Name, unmangledName, sizeof(unmangledName), UNDNAME_COMPLETE);

            stackTrace.append( unmangledName );
            stackTrace.append( "\n" );
        } else {
            stackTrace.append( "<Unknown>\n" );
        }
    }

    ::SymCleanup( ::GetCurrentProcess() );
#endif

    return Intern(stackTrace.c_str());
}
