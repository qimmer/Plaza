//
// Created by Kim Johannsen on 28-02-2018.
//

#ifndef PLAZA_VIRTUALPATH_H
#define PLAZA_VIRTUALPATH_H


#include <Core/Entity.h>

DeclareComponent(VirtualPath)

DeclareComponentPropertyReactive(VirtualPath, StringRef, VirtualPathTrigger)
DeclareComponentPropertyReactive(VirtualPath, StringRef, VirtualPathDestination)

void ResolveVirtualPath(StringRef virtualPath, char *resolvedPath);

#endif //PLAZA_VIRTUALPATH_H
