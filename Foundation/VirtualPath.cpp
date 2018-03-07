//
// Created by Kim Johannsen on 28-02-2018.
//

#include <Core/String.h>
#include "VirtualPath.h"

struct VirtualPath {
    String VirtualPathTrigger, VirtualPathDestination;
};

DefineComponent(VirtualPath)
    DefineProperty(StringRef, VirtualPathTrigger)
    DefineProperty(StringRef, VirtualPathDestination)
EndComponent()


DefineComponentProperty(VirtualPath, StringRef, VirtualPathTrigger)
DefineComponentProperty(VirtualPath, StringRef, VirtualPathDestination)

StringRef ResolveVirtualPath(StringRef virtualPath) {
    auto len = strlen(virtualPath);
    for_entity(entity, HasVirtualPath) {
        auto data = GetVirtualPath(entity);
        auto triggerLen = data->VirtualPathTrigger.length();

        if(len < triggerLen) continue;
        if(virtualPath[0] == data->VirtualPathTrigger[0] && virtualPath[triggerLen - 1] == data->VirtualPathTrigger[triggerLen - 1]) {
            if(memcmp(data->VirtualPathTrigger.c_str(), virtualPath, triggerLen) == 0) {
                return FormatString("%s%s", data->VirtualPathDestination.c_str(), &virtualPath[triggerLen]);
            }
        }
    }

    return virtualPath;
}