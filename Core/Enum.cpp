//
// Created by Kim Johannsen on 20-03-2018.
//

#include <cstring>
#include "Enum.h"
#include "Types.h"
#include "Identification.h"


struct Enum {
    bool EnumCombinable;
};

struct EnumFlag {
    u64 EnumFlagValue;
};

API_EXPORT StringRef GetEnumName(Entity e, u64 value) {
    for_children(flag, EnumFlags, e) {
        if(GetEnumFlagValue(flag) == value) {
            return GetName(flag);
        }
    }

    return "";
}

API_EXPORT u64 GetEnumValue(Entity e, StringRef name) {
    name = Intern(name);
    for_children(flag, EnumFlags, e) {
        if(GetName(flag) == name) {
            return GetEnumFlagValue(flag);
        }
    }

    return 0;
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
