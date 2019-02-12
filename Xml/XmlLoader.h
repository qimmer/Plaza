//
// Created by Kim Johannsen on 07/02/2018.
//

#ifndef PLAZA_XMLLOADER_H
#define PLAZA_XMLLOADER_H

Unit(JsonPersistance)

bool SerializeXml(Entity stream, Entity entity, s16 includeChildLevels = 100, s16 includeReferenceLevels = 0, bool includePersistedChildren = true, bool includeNativeEntities = true);
bool DeserializeXml(Entity stream, Entity entity);

Function(DeserializeJsonFromString, bool, Entity stream, Entity entity, StringRef jsonString)

#endif //PLAZA_XMLLOADER_H
