//
// Created by Kim Johannsen on 28-02-2018.
//

#ifndef PLAZA_VIRTUALPATH_H
#define PLAZA_VIRTUALPATH_H


#include <Core/Entity.h>

Unit(VirtualPath)
    Component(VirtualPath)
        Property(StringRef, VirtualPathTrigger)
        Property(StringRef, VirtualPathDestination)

void ResolveVirtualPath(StringRef virtualPath, u32 bufferLength, char *resolvedPath);

#endif //PLAZA_VIRTUALPATH_H
