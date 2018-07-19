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

void CalculateEntityPath(char *dest, size_t bufMax, Entity entity);
StringRef GetEntityRelativePath(StringRef entityPath, Entity relativeTo);
Entity FindEntityByPath(StringRef path);

#define PathMax 2048

#endif //PLAZA_IDENTIFICATION_H
