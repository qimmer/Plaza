//
// Created by Kim Johannsen on 28-02-2018.
//

#ifndef PLAZA_VIRTUALPATH_H
#define PLAZA_VIRTUALPATH_H

#include <Core/Entity.h>

struct VirtualPath {
    StringRef VirtualPathTrigger;
    StringRef VirtualPathDestination;
};

Unit(VirtualPath)
    Component(VirtualPath)
        Property(StringRef, VirtualPathTrigger)
        Property(StringRef, VirtualPathDestination)

StringRef ResolveVirtualPath(StringRef virtualPath);

#endif //PLAZA_VIRTUALPATH_H
