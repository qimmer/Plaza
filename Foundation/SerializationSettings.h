//
// Created by Kim on 17-09-2018.
//

#ifndef PLAZA_SERIALIZATIONSETTINGS_H
#define PLAZA_SERIALIZATIONSETTINGS_H

#include <Core/NativeUtils.h>

Unit(SerializationSettings)
    Component(SerializationSettings)
        Property(u32, SerializationSettingsMaxChildLevel)
        Property(u32, SerializationSettingsMaxReferenceLevel)
        Property(bool, SerializationSettingsExcludePersistedChildren)
        Property(bool, SerializationSettingsExcludeNativeEntities)
        Property(bool, SerializationSettingsIncludeDrivers)

#endif //PLAZA_SERIALIZATIONSETTINGS_H
