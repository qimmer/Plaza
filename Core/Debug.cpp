//
// Created by Kim Johannsen on 21/12/2017.
//

#include <Core/Debug.h>
#include <Core/NativeUtils.h>
#include <Core/Event.h>

#include <stdio.h>
#include <stdarg.h>
#include <cstring>
#include <process.h>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <signal.h>
#endif

#ifdef WIN32
#include <Windows.h>
#include "Identification.h"

#endif

BeginUnit(Debug)
    RegisterEvent(LogMessageReceived)
EndUnit()

API_EXPORT void Log(Entity context, int severity, StringRef format, ...) {
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

    setbuf(stdout, 0);
    printf("%s", buffer);

    //FireEvent(EventOf_LogMessageReceived(), context, buffer, severity);

    if(severity >= LogSeverity_Error) {
        DebuggerBreak();
    }
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

        auto properties = GetProperties(component);
        for(auto i = 0; i < GetNumProperties(component); ++i) {
            auto property = properties[i];
            Entity value = 0;
            GetPropertyValue(property, entity, &value);
            switch(GetPropertyKind(property)) {
                case PropertyKind_Child:
                    printf("%*s%s: ", (level + 1) * identation, " ", GetDebugName(property));
                    PrintNode(level+1, value);
                    break;
                case PropertyKind_Array:
                    printf("%*s%s: [\n", (level + 1) * identation, " ", GetDebugName(property));
                    auto elements = GetArrayPropertyElements(property, entity);
                    for(auto j = 0; j < GetArrayPropertyCount(property, entity); ++j) {
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
