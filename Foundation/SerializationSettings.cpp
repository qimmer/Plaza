//
// Created by Kim on 17-09-2018.
//

#include "SerializationSettings.h"

struct SerializationSettings {
    u32 SerializationSettingsMaxChildLevel;
    u32 SerializationSettingsMaxReferenceLevel;
    bool SerializationSettingsExcludePersistedChildren;
    bool SerializationSettingsExcludeNativeEntities;
    bool SerializationSettingsIncludeDrivers;
};

BeginUnit(SerializationSettings)
    BeginComponent(SerializationSettings)
        RegisterProperty(u32, SerializationSettingsMaxChildLevel)
        RegisterProperty(u32, SerializationSettingsMaxReferenceLevel)
        RegisterProperty(bool, SerializationSettingsExcludePersistedChildren)
        RegisterProperty(bool, SerializationSettingsExcludeNativeEntities)
        RegisterProperty(bool, SerializationSettingsIncludeDrivers)
    EndComponent()
EndUnit()