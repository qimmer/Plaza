//
// Created by Kim Johannsen on 28-02-2018.
//

#ifndef PLAZA_VIRTUALPATH_H
#define PLAZA_VIRTUALPATH_H


#include <Core/Entity.h>

DeclareComponent(VirtualPath)

DeclareComponentProperty(VirtualPath, StringRef, VirtualPathTrigger)
DeclareComponentProperty(VirtualPath, StringRef, VirtualPathDestination)

StringRef ResolveVirtualPath(StringRef virtualPath);

#endif //PLAZA_VIRTUALPATH_H
