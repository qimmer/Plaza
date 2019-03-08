//
// Created by Kim on 07-11-2018.
//

#include "IniPersistance.h"
#include <Foundation/NativeUtils.h>
#include <Foundation/Stream.h>
#include <Core/Debug.h>
#include <Core/Identification.h>
#include <Json/NativeUtils.h>

#include <sstream>

API_EXPORT bool SerializeIni(Entity stream) {
    std::stringstream ss;

    auto& iniSettingGroups = GetIniFileSettingGroups(stream);
    for(auto i = 0; i < iniSettingGroups.size(); ++i) {
        auto settingGroup = iniSettingGroups[i];

        auto data = GetIniSettingGroupData(settingGroup);
        ss << "[" << GetUuid(data->IniSettingGroupEntity) << "]" << std::endl;

        auto& iniSettings = GetIniSettingGroupSettings(settingGroup);
        for(auto j = 0; j < iniSettings.size(); ++j) {
            auto setting = iniSettings[j];
            auto property = GetIniSettingProperty(setting);

            auto value = GetPropertyValue(setting, data->IniSettingGroupEntity);
            ss << GetUuid(property) << "=" << Cast(value, TypeOf_StringRef).as_StringRef << std::endl;
        }

        ss << std::endl;
    }

    bool result = true;
    auto str = ss.str();
    bool streamWasOpen = IsStreamOpen(stream);
    if(!streamWasOpen) {
        Assert(stream, StreamOpen(stream, StreamMode_Write));
    }

    auto numWritten = StreamWrite(stream, str.length(), str.c_str());
    if(numWritten != str.length()) {
        result = false;
    }

    if(!streamWasOpen) {
        StreamClose(stream);
    }

    return result;
}

API_EXPORT bool DeserializeIni(Entity stream) {
    bool streamWasOpen = IsStreamOpen(stream);
    auto startOffset = 0;
    if(!streamWasOpen) {
        if(!StreamOpen(stream, StreamMode_Read)) {
            Log(stream, LogSeverity_Error, "Could not open stream '%s' for deserialization.", GetStreamPath(stream));
            return false;
        }
    } else {
        startOffset = StreamTell(stream);
    }

    Assert(stream, StreamSeek(stream, StreamSeek_End));
    auto size = StreamTell(stream);
    char *data = (char*)malloc(size + 1);
    Assert(stream, data);
    Assert(stream, StreamSeek(stream, startOffset));
    Assert(stream, size == StreamRead(stream, size, data));

    if(!streamWasOpen) {
        StreamClose(stream);
    }

    data[size] = '\0';

    SetNumIniFileSettingGroups(stream, 0);

    std::stringstream ss(data);
    std::string str;
    Entity currentEntity = 0, currentGroup = 0;
    while(std::getline(ss, str)) {
        if(str.length() && str.c_str()[str.length() - 1] == '\r') str.pop_back();

        char uuid[1024];
        char value[1024];
        if(str.length() >= 1023) continue;
        if(sscanf(str.c_str(), "[%[^]]", uuid)) {
            currentEntity = FindEntityByUuid(uuid);
            if(IsEntityValid(currentEntity)) {
                currentGroup = AddIniFileSettingGroups(stream);
                SetIniSettingGroupEntity(currentGroup, currentEntity);
            }
        }

        if(sscanf(str.c_str(), "%1023[^=]=%1023[^=\r]", uuid, value) > 1 && IsEntityValid(currentEntity) && IsEntityValid(currentGroup)) {
            auto property = FindEntityByUuid(StringFormatV("Property.%s", uuid));
            if(IsEntityValid(property)) {
                Variant valueVar;
                valueVar.as_StringRef = value;
                valueVar.type = TypeOf_StringRef;

                auto enu = GetPropertyEnum(property);
                if(IsEntityValid(enu)) {
                    valueVar.as_u64 = GetEnumValue(enu, StringFormatV("%s.%s", GetUuid(enu), value));
                    valueVar.type = TypeOf_u64;
                }

                valueVar = Cast(valueVar, GetPropertyType(property));
                if(valueVar.type) {
                    SetPropertyValue(property, currentEntity, valueVar);
                }

                auto setting = AddIniSettingGroupSettings(currentGroup);
                SetIniSettingProperty(setting, property);
            }
        }
    }

    free(data);

    return true;
}

static bool Serialize(Entity persistancePoint) {
    return SerializeIni(persistancePoint);
}

static bool Deserialize(Entity persistancePoint) {
    return DeserializeIni(persistancePoint);
}

BeginUnit(IniPersistance)
    BeginComponent(IniSetting)
        RegisterReferenceProperty(Property, IniSettingProperty)
    EndComponent()
    BeginComponent(IniSettingGroup)
        RegisterProperty(Entity, IniSettingGroupEntity)
        RegisterArrayProperty(IniSetting, IniSettingGroupSettings)
    EndComponent()
    BeginComponent(IniFile)
        RegisterArrayProperty(IniSettingGroup, IniFileSettingGroups)
    EndComponent()

    RegisterSerializer(IniSerializer, "application/ini")

    ModuleData(
            {
                "FileTypes": [
                    { "Uuid": "FileType.Ini", "FileTypeExtension": ".ini", "FileTypeMimeType" : "application/ini" }
                ]
            }
    );
EndUnit()