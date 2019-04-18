//
// Created by Kim on 18-07-2018.
//

#ifndef PLAZA_IDENTIFICATION_H
#define PLAZA_IDENTIFICATION_H

#include <Core/NativeUtils.h>

struct Identification {
	StringRef Uuid;
};

Unit(Identification)
    Component(Identification)
        Property(StringRef, Uuid)

StringRef GetEntityRelativePath(StringRef entityPath, Entity relativeTo, bool preferNameToIndex);
Entity FindEntityByUuid(StringRef uuid);

bool GetParentPath(StringRef childPath, u32 bufLen, char *parentPath);
StringRef CreateGuidFromPath(StringRef path);

StringRef CreateGuid();

#define PathMax 512

#endif //PLAZA_IDENTIFICATION_H
