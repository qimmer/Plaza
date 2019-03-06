//
// Created by Kim on 18-07-2018.
//

#ifndef PLAZA_IDENTIFICATION_H
#define PLAZA_IDENTIFICATION_H

#include <Core/NativeUtils.h>

Unit(Identification)
    Component(Identification)
        static StringRef __Uuid__Meta = "";
        Declare(Property, Uuid)
        struct UuidChangedArgs { Entity ChangedEntity; StringRef OldValue; StringRef NewValue; };
        StringRef GetUuid(Entity entity);
        void SetUuid(Entity entity, StringRef value);

StringRef GetEntityRelativePath(StringRef entityPath, Entity relativeTo, bool preferNameToIndex);
Entity FindEntityByUuid(StringRef uuid);

bool GetParentPath(StringRef childPath, u32 bufLen, char *parentPath);
StringRef CreateGuidFromPath(StringRef path);

Function(CreateGuid, StringRef)

#define PathMax 512

#endif //PLAZA_IDENTIFICATION_H
