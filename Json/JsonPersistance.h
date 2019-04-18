//
// Created by Kim Johannsen on 20/02/2018.
//

#ifndef PLAZA_JSONPERSISTANCEPOINT_H
#define PLAZA_JSONPERSISTANCEPOINT_H

#include <Core/NativeUtils.h>
#include <Foundation/PersistancePoint.h>
#include <Foundation/NativeUtils.h>

struct JsonSettings {
    u8 JsonSettingsMaxRecursiveLevels;
    bool JsonSettingsResolveBindings, JsonSettingsIgnoreReadOnly, JsonSettingsExplicitComponents, JsonSettingsExplicitBindings, JsonSettingsExplicitHandle;
};

Unit(JsonPersistance)
    Component(JsonSettings)
        Property(bool, JsonSettingsResolveBindings)
        Property(bool, JsonSettingsIgnoreReadOnly)
        Property(u8, JsonSettingsMaxRecursiveLevels)
        Property(bool, JsonSettingsExplicitComponents)
        Property(bool, JsonSettingsExplicitBindings)
        Property(bool, JsonSettingsExplicitHandle)

        Serializer(Json)

bool SerializeJson(Entity stream, Entity entity, Entity jsonSettings);
bool DeserializeJson(Entity stream, Entity entity);

Function(DeserializeJsonFromString, bool, Entity stream, Entity entity, StringRef jsonString)

#endif //PLAZA_JSONPERSISTANCEPOINT_H
