//
// Created by Kim Johannsen on 20-03-2018.
//

#include <cstring>
#include "Enum.h"

struct EnumValue {
    StringRef Name;
    u64 Value;
};

struct EnumData {
    EnumData() : Combinable(false) {}

    Vector<EnumValue> Values;
    bool Combinable;
};

DefineHandle(Enum, EnumData)

void SetEnumCombinable(Enum e, bool combinable) {
    EnumAt(e)->Combinable = combinable;
}

bool GetEnumCombinable(Enum e) {
    return EnumAt(e)->Combinable;
}


void SetEnumFlag(Enum e, StringRef name, u64 value) {
    EnumValue v;
    v.Name = name;
    v.Value = value;

    EnumAt(e)->Values.push_back(v);
}


u32 GetEnumFlags(Enum e) {
    return EnumAt(e)->Values.size();
}

StringRef GetEnumFlagName(Enum e, u32 flagIndex) {
    if(flagIndex == (u32)-1) {
        return "<Unknown>";
    }

    return EnumAt(e)->Values[flagIndex].Name;
}

u64 GetEnumFlagValue(Enum e, u32 flagIndex) {
    return EnumAt(e)->Values[flagIndex].Value;
}

u32 GetEnumFlagIndexByValue(Enum e, u64 flagValue) {
    for(auto i = 0; i < EnumAt(e)->Values.size(); ++i) {
        if(EnumAt(e)->Values[i].Value == flagValue) return i;
    }
    return 0;
}

u32 GetEnumFlagIndexByName(Enum e, StringRef name) {
    for(auto i = 0; i < EnumAt(e)->Values.size(); ++i) {
        if(strcmp(EnumAt(e)->Values[i].Name, name)) return i;
    }
    return 0;
}
