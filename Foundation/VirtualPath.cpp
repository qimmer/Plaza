//
// Created by Kim Johannsen on 28-02-2018.
//

#include <Core/String.h>
#include "VirtualPath.h"

struct VirtualPath {
    String VirtualPathTrigger, VirtualPathDestination;
};

DefineComponent(VirtualPath)
    DefinePropertyReactive(StringRef, VirtualPathTrigger)
    DefinePropertyReactive(StringRef, VirtualPathDestination)
EndComponent()


DefineComponentPropertyReactive(VirtualPath, StringRef, VirtualPathTrigger)
DefineComponentPropertyReactive(VirtualPath, StringRef, VirtualPathDestination)

void ResolveVirtualPath(StringRef virtualPath, char *resolvedPath) {
    auto len = strlen(virtualPath);
    for_entity(entity, VirtualPath) {
        auto data = GetVirtualPath(entity);
        auto triggerLen = data->VirtualPathTrigger.length();

        if(len < triggerLen) continue;
        if(virtualPath[0] == data->VirtualPathTrigger[0] && virtualPath[triggerLen - 1] == data->VirtualPathTrigger[triggerLen - 1]) {
            if(memcmp(data->VirtualPathTrigger.c_str(), virtualPath, triggerLen) == 0) {
                snprintf(resolvedPath, PATH_MAX, "%s%s", data->VirtualPathDestination.c_str(), &virtualPath[triggerLen]);
                return;
            }
        }
    }

    strcpy(resolvedPath, virtualPath);
}
