//
// Created by Kim Johannsen on 28-02-2018.
//

#include "VirtualPath.h"
#include <cstring>
#include <cstdio>

API_EXPORT StringRef ResolveVirtualPath(StringRef virtualPath) {
    auto len = virtualPath ? strlen(virtualPath) : 0;
    VirtualPath data;
    for_entity_data(entity, ComponentOf_VirtualPath(), &data) {
        auto triggerLen = strlen(data.VirtualPathTrigger);

        if(len < triggerLen) continue;
        if(virtualPath[0] == data.VirtualPathTrigger[0] && virtualPath[triggerLen - 1] == data.VirtualPathTrigger[triggerLen - 1]) {
            if(memcmp(data.VirtualPathTrigger, virtualPath, triggerLen) == 0) {
                return StringFormatV("%s%s", data.VirtualPathDestination, &virtualPath[triggerLen]);
            }
        }
    }

    return virtualPath;
}

BeginUnit(VirtualPath)
    BeginComponent(VirtualPath)
        RegisterProperty(StringRef, VirtualPathTrigger)
        RegisterProperty(StringRef, VirtualPathDestination)
    EndComponent()
EndUnit()
