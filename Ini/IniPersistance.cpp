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

    auto fileData = GetIniFile(stream);
    for(auto settingGroup : fileData.IniFileSettingGroups) {
        auto data = GetIniSettingGroup(settingGroup);
        ss << "[" << GetIdentification(data.IniSettingGroupEntity).Uuid << "]" << std::endl;

        for(auto setting : data.IniSettingGroupSettings) {
            auto property = GetIniSetting(setting).IniSettingProperty;

            auto value = GetPropertyValue(setting, data.IniSettingGroupEntity);
            ss << GetIdentification(property).Uuid << "=" << Cast(value, TypeOf_StringRef).as_StringRef << std::endl;
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
            Log(stream, LogSeverity_Error, "Could not open stream '%s' for deserialization.", GetStream(stream).StreamPath);
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

    auto fileData = GetIniFile(stream);
    fileData.IniFileSettingGroups.SetSize(0);

    std::stringstream ss(data);
    std::string str;
    Entity currentEntity = 0, currentGroup = 0;
    IniSettingGroup groupData;
    while(std::getline(ss, str)) {
        if(str.length() && str.c_str()[str.length() - 1] == '\r') str.pop_back();

        char uuid[1024];
        char value[1024];
        if(str.length() >= 1023) continue;
        if(sscanf(str.c_str(), "[%[^]]", uuid)) {
            currentEntity = FindEntityByUuid(uuid);
            if(IsEntityValid(currentEntity)) {
                currentGroup = CreateEntity();
                groupData = GetIniSettingGroup(currentGroup);
                groupData.IniSettingGroupEntity = currentEntity;
                fileData.IniFileSettingGroups.Add(currentGroup);
            }
        }

        if(sscanf(str.c_str(), "%1023[^=]=%1023[^=\r]", uuid, value) > 1 && IsEntityValid(currentEntity) && IsEntityValid(currentGroup)) {
            auto property = FindEntityByUuid(StringFormatV("Property.%s", uuid));
            if(IsEntityValid(property)) {
                Variant valueVar;
                valueVar.as_StringRef = value;
                valueVar.type = TypeOf_StringRef;

                auto enu = GetProperty(property).PropertyEnum;
                if(IsEntityValid(enu)) {
                    valueVar.as_u64 = GetEnumValue(enu, StringFormatV("%s.%s", GetIdentification(enu).Uuid, value));
                    valueVar.type = TypeOf_u64;
                }

                valueVar = Cast(valueVar, GetProperty(property).PropertyType);
                if(valueVar.type) {
                    SetPropertyValue(property, currentEntity, valueVar);
                }

                auto setting = CreateEntity();
                SetIniSetting(setting, {property});
                groupData.IniSettingGroupSettings.Add(setting);
            }
        }

        SetIniSettingGroup(currentGroup, groupData);
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

    RegisterSerializer(Ini, "application/ini")

    ModuleData(
            {
                "FileTypes": [
                    { "Uuid": "FileType.Ini", "FileTypeExtension": ".ini", "FileTypeMimeType" : "application/ini" }
                ]
            }
    );
EndUnit()