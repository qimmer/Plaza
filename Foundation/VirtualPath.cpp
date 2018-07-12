//
// Created by Kim Johannsen on 28-02-2018.
//

#include "VirtualPath.h"
#include <cstring>
#include <cstdio>

struct VirtualPath {
    char VirtualPathTrigger[32];
    char VirtualPathDestination[512 - 32];
};

API_EXPORT void ResolveVirtualPath(StringRef virtualPath, u32 bufferLength, char *resolvedPath) {
    auto len = strlen(virtualPath);
    for_entity(entity, data, VirtualPath) {
        auto triggerLen = strlen(data->VirtualPathTrigger);

        if(len < triggerLen) continue;
        if(virtualPath[0] == data->VirtualPathTrigger[0] && virtualPath[triggerLen - 1] == data->VirtualPathTrigger[triggerLen - 1]) {
            if(memcmp(data->VirtualPathTrigger, virtualPath, triggerLen) == 0) {
                snprintf(resolvedPath, bufferLength, "%s%s", data->VirtualPathDestination, &virtualPath[triggerLen]);
                return;
            }
        }
    }

    strcpy(resolvedPath, virtualPath);
}

BeginUnit(VirtualPath)
    BeginComponent(VirtualPath)
        RegisterProperty(StringRef, VirtualPathTrigger)
        RegisterProperty(StringRef, VirtualPathDestination)
    EndComponent()
EndUnit()
