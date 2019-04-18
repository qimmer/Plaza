//
// Created by Kim on 07-11-2018.
//

#ifndef PLAZA_INIPERSISTANCE_H
#define PLAZA_INIPERSISTANCE_H

#include <Core/NativeUtils.h>
#include <Foundation/PersistancePoint.h>
#include <Foundation/NativeUtils.h>

struct IniSetting {
    Entity IniSettingProperty;
};

struct IniSettingGroup {
    Entity IniSettingGroupEntity;
    ChildArray IniSettingGroupSettings;
};

struct IniFile {
    ChildArray IniFileSettingGroups;
};

Unit(IniPersistance)
    Component(IniSetting)
        ReferenceProperty(Property, IniSettingProperty)

    Component(IniSettingGroup)
        Property(Entity, IniSettingGroupEntity)
        ArrayProperty(IniSetting, IniSettingGroupSettings)

    Component(IniFile)
        ArrayProperty(IniSettingGroup, IniFileSettingGroups)

    Serializer(Ini)

bool SerializeIni(Entity stream, Entity entity);
bool DeserializeIni(Entity stream, Entity entity);

#endif //PLAZA_INIPERSISTANCE_H
