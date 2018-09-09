//
// Created by Kim Johannsen on 20/02/2018.
//

#ifndef PLAZA_JSONPERSISTANCEPOINT_H
#define PLAZA_JSONPERSISTANCEPOINT_H

#include <Core/NativeUtils.h>
#include <Foundation/PersistancePoint.h>

Unit(JsonPersistance)

bool SerializeJson(Entity stream, Entity entity, s16 includeChildLevels = 100, s16 includeReferenceLevels = 0, bool includePersistedChildren = true, bool includeNativeEntities = true);
bool DeserializeJson(Entity stream, Entity entity);

Function(DeserializeJsonFromString, bool, Entity stream, Entity entity, StringRef jsonString)

#endif //PLAZA_JSONPERSISTANCEPOINT_H
