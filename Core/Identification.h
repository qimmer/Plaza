//
// Created by Kim on 18-07-2018.
//

#ifndef PLAZA_IDENTIFICATION_H
#define PLAZA_IDENTIFICATION_H

#include <Core/NativeUtils.h>

Unit(Identification)
    Component(Identification)
        static StringRef __Name__Meta = "";
        Declare(Property, Name)
        struct NameChangedArgs { Entity ChangedEntity; StringRef OldValue; StringRef NewValue; };
        Event(NameChanged)
        StringRef GetName(Entity entity);
        void SetName(Entity entity, StringRef value);
        static StringRef __Uuid__Meta = "";
        Declare(Property, Uuid)
        struct UuidChangedArgs { Entity ChangedEntity; StringRef OldValue; StringRef NewValue; };
        Event(UuidChanged)
        StringRef GetUuid(Entity entity);
        void SetUuid(Entity entity, StringRef value);

StringRef CalculateEntityPath(Entity entity, bool preferNameToIndices);
StringRef GetEntityRelativePath(StringRef entityPath, Entity relativeTo, bool preferNameToIndex);
Entity FindEntityByPath(StringRef path);
Entity FindEntityByUuid(StringRef uuid);
Entity FindEntityByName(Entity component, StringRef typeName);

bool GetParentPath(StringRef childPath, u32 bufLen, char *parentPath);
StringRef CreateGuidFromPath(StringRef path);

Function(CreateGuid, StringRef)

#define PathMax 512

#endif //PLAZA_IDENTIFICATION_H
