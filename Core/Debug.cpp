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
#include "Identification.h"
#include "CoreModule.h"
#include <Core/Date.h>

#endif

#include <EASTL/map.h>

struct LogMessage {
    StringRef LogMessageEntity;
    StringRef LogMessageText;
    u8 LogMessageSeverity;
    u32 LogMessageCount;
    Date LogMessageTime;
};

API_EXPORT void Log(Entity context, int severity, StringRef format, ...) {
    char buffer[4096];
    va_list arg;

    auto numWritten = 0;
    va_start(arg, format);
    numWritten += vsnprintf(buffer + numWritten, 4095, format, arg);
    va_end(arg);

    buffer[numWritten] = '\0';

#ifdef DEBUG
    setbuf(stdout, 0);
    printf("%s", buffer);
#endif

    auto messageText = Intern(buffer);
    auto uuid = GetUuid(context);
    for_entity(message, data, LogMessage) {
        if(messageText == data->LogMessageText && data->LogMessageEntity == uuid) {
            SetLogMessageCount(message, data->LogMessageCount + 1);
            SetLogMessageTime(message, GetDateNow());
            return;
        }
    }

    message = AddLogMessages(ModuleOf_Core());
    SetLogMessageEntity(message, uuid);
    SetLogMessageText(message, messageText);
    SetLogMessageTime(message, GetDateNow());
    SetLogMessageSeverity(message, severity);

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

BeginUnit(Debug)
    BeginComponent(LogMessage)
        RegisterProperty(StringRef, LogMessageEntity)
        RegisterProperty(StringRef, LogMessageText)
        RegisterProperty(u8, LogMessageSeverity)
        RegisterProperty(u32, LogMessageCount)
        RegisterProperty(Date, LogMessageTime)
    EndComponent()
EndUnit()