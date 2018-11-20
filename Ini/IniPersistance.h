//
// Created by Kim on 07-11-2018.
//

#ifndef PLAZA_INIPERSISTANCE_H
#define PLAZA_INIPERSISTANCE_H

#include <Core/NativeUtils.h>
#include <Foundation/PersistancePoint.h>

struct IniSetting {
    Entity IniSettingProperty;
};

struct IniSettingGroup {
    Entity IniSettingGroupEntity;
};

struct IniFile {};

Unit(IniPersistance)
    Component(IniSetting)
        ReferenceProperty(Property, IniSettingProperty)

    Component(IniSettingGroup)
        Property(Entity, IniSettingGroupEntity)
        ArrayProperty(IniSetting, IniSettingGroupSettings)

    Component(IniFile)
        ArrayProperty(IniSettingGroup, IniFileSettingGroups)

bool SerializeIni(Entity stream, Entity entity);
bool DeserializeIni(Entity stream, Entity entity);

#endif //PLAZA_INIPERSISTANCE_H
