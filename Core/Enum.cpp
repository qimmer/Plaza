//
// Created by Kim Johannsen on 20-03-2018.
//

#include <cstring>
#include "Enum.h"
#include "Types.h"
#include "Identification.h"


API_EXPORT StringRef GetEnumName(Entity e, u64 value) {
    for_children(flag, PropertyOf_EnumFlags(), e) {
        if(GetEnumFlag(flag).EnumFlagValue == value) {
            return GetArrayChild(flag).Name;
        }
    }

    return 0;
}

API_EXPORT u64 GetEnumValue(Entity e, StringRef name) {
    if(!name) return 0;

    name = Intern(name);

    auto uuid = StringFormatV("%s.%s", GetIdentification(e).Uuid, name);

    for_children(flag, PropertyOf_EnumFlags(), e) {
        if(GetIdentification(flag).Uuid == name || GetIdentification(flag).Uuid == uuid) {
            return GetEnumFlag(flag).EnumFlagValue;
        }
    }

    return -1;
}

BeginUnit(Enum)
    BeginComponent(Enum)
        RegisterProperty(bool, EnumCombinable)
        RegisterArrayProperty(EnumFlag, EnumFlags)
    EndComponent()
    BeginComponent(EnumFlag)
        RegisterProperty(u64, EnumFlagValue)
    EndComponent()
EndUnit()
