
#ifndef PersistancePoint_H
#define PersistancePoint_H

#include <Core/NativeUtils.h>

Unit(PersistancePoint)
    Component(PersistancePoint)
        Property(bool, PersistancePointAsync)
        Property(bool, PersistancePointLoaded)
        Property(bool, PersistancePointLoading, PropertyFlag_ReadOnly, PropertyFlag_Transient)
        Property(bool, PersistancePointSaving, PropertyFlag_ReadOnly, PropertyFlag_Transient)
        Property(Entity, PersistancePointSerializer, PropertyFlag_ReadOnly, PropertyFlag_Transient)

    Component(Serializer)
        Property(StringRef, SerializerMimeType)

    Function(Save, bool, Entity persistancePoint)

struct Serializer {
    char SerializerMimeType[64];
    bool(*SerializeHandler)(Entity entity);
    bool(*DeserializeHandler)(Entity entity);
};

#define RegisterSerializer(NAME, MIMETYPE) \
    auto NAME ## serializer = CreateEntityFromName(module, #NAME "Serializer"); \
    SetSerializerMimeType(NAME ## serializer, MIMETYPE); \
    auto NAME ## serializerData = GetSerializerData(NAME ## serializer); \
    NAME ## serializerData->SerializeHandler = Serialize; \
    NAME ## serializerData->DeserializeHandler = Deserialize;

#endif //PersistancePoint_H
