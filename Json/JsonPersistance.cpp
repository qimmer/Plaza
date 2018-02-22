//
// Created by Kim Johannsen on 20/02/2018.
//

#include <Foundation/Serialization.h>
#include "JsonPersistance.h"
#include <Core/Hierarchy.h>
#include <Foundation/Persistance.h>
#include <Foundation/Stream.h>
#include <Foundation/StreamedPersistancePoint.h>
#include <Foundation/PersistancePoint.h>
#include <File/FileStream.h>

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

using namespace rapidjson;

#define DeclareGetSet(TYPE) typedef TYPE (*TYPE ## Getter)(Entity entity); typedef void(*TYPE ## Setter)(Entity entity, TYPE value);

#define WriteIf(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        writer.JSONWRITEFUNC(((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity));\
    } do{} while(false)

#define ReadIf(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, reader.Get ## JSONREADFUNC ());\
    } do{} while(false)

#define WriteReferencedIf(TYPE, JSONWRITEFUNC, CONVERTERFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        writer.JSONWRITEFUNC(CONVERTERFUNC(((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity)));\
    } do{} while(false)

#define ReadReferencedIf(TYPE, JSONREADFUNC, CONVERTERFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, CONVERTERFUNC(reader.Get ## JSONREADFUNC ()));\
    } do{} while(false)

#define ReadVec2If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, v);\
    }

#define ReadVec3If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, v);\
    }

#define ReadVec4If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        v.w = reader.GetObject()["w"].Get ## JSONREADFUNC ();\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, v);\
    }

#define WriteVec2If(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        auto v = ((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity);\
        writer.StartObject();\
        writer.String("x");\
        writer.JSONWRITEFUNC(v.x);\
        writer.String("y");\
        writer.JSONWRITEFUNC(v.y);\
        writer.EndObject();\
    } do{} while(false)

#define WriteVec3If(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        auto v = ((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity);\
        writer.StartObject();\
        writer.String("x");\
        writer.JSONWRITEFUNC(v.x);\
        writer.String("y");\
        writer.JSONWRITEFUNC(v.y);\
        writer.String("z");\
        writer.JSONWRITEFUNC(v.z);\
        writer.EndObject();\
    } do{} while(false)

#define WriteVec4If(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        auto v = ((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity);\
        writer.StartObject();\
        writer.String("x");\
        writer.JSONWRITEFUNC(v.x);\
        writer.String("y");\
        writer.JSONWRITEFUNC(v.y);\
        writer.String("z");\
        writer.JSONWRITEFUNC(v.z);\
        writer.String("w");\
        writer.JSONWRITEFUNC(v.w);\
        writer.EndObject();\
    } do{} while(false)

DefineService(JsonPersistance)
EndService()

DeclareGetSet(u8)
DeclareGetSet(u16)
DeclareGetSet(u32)
DeclareGetSet(u64)
DeclareGetSet(s8)
DeclareGetSet(s16)
DeclareGetSet(s32)
DeclareGetSet(s64)
DeclareGetSet(StringRef)
DeclareGetSet(float)
DeclareGetSet(double)
DeclareGetSet(bool)
DeclareGetSet(v2f)
DeclareGetSet(v3f)
DeclareGetSet(v4f)
DeclareGetSet(v2i)
DeclareGetSet(v3i)
DeclareGetSet(v4i)
DeclareGetSet(Entity)
DeclareGetSet(Type)

static void SerializeJson(Entity persistancePoint, Entity stream) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    writer.StartObject();
    for(auto persistentEntity = GetNextEntity(0); IsEntityValid(persistentEntity); persistentEntity = GetNextEntity(persistentEntity)) {
        if(!HasPersistance(persistentEntity) || GetEntityPersistancePoint(persistentEntity) != persistancePoint) continue;

        writer.String(GetEntityPath(persistentEntity));
        writer.StartObject();

        for(auto property = GetNextProperty(0); IsPropertyValid(property); property = GetNextProperty(property)) {
            auto componentType = GetPropertyOwner(property);
            if(!HasComponent(persistentEntity, componentType)) {
                continue;
            }

            auto type = GetPropertyType(property);

            writer.String(GetPropertyName(property));

            WriteIf(u8, Uint);
            WriteIf(u16, Uint);
            WriteIf(u32, Uint);
            WriteIf(u64, Uint64);
            WriteIf(s8, Int);
            WriteIf(s16, Int);
            WriteIf(s32, Int);
            WriteIf(s64, Int64);
            WriteIf(StringRef, String);
            WriteIf(float, Double);
            WriteIf(double, Double);
            WriteIf(bool, Bool);
            WriteReferencedIf(Entity, String, GetEntityPath);
            WriteReferencedIf(Type, String, GetTypeName);
            WriteVec2If(v2f, Double);
            WriteVec3If(v3f, Double);
            WriteVec4If(v4f, Double);
            WriteVec2If(v2i, Int);
            WriteVec3If(v3i, Int);
            WriteVec4If(v4i, Int);
        }

        writer.EndObject();
    }
    writer.EndObject();

    writer.Flush();

    StreamOpen(stream);
    StreamWrite(stream, buffer.GetLength(), buffer.GetString());
    StreamClose(stream);
}

static void DeserializeJson(Entity persistancePoint, Entity stream) {
    StreamOpen(stream);
    StreamSeek(stream, STREAM_SEEK_END);
    auto size = StreamTell(stream);
    char *data = (char*)alloca(size + 1);
    StreamSeek(stream, 0);
    StreamRead(stream, size, data);
    data[size] = '\0';

    Document document;
    document.Parse(data, size);

    for (Value::ConstMemberIterator entityIterator = document.MemberBegin();
         entityIterator != document.MemberEnd(); ++entityIterator)
    {
        auto entityPath = entityIterator->name.GetString();
        auto persistentEntity = CreateEntityFromPath(entityPath);
        SetEntityPersistancePoint(persistentEntity, persistancePoint);

        for (Value::ConstMemberIterator propertyIterator = entityIterator->value.MemberBegin();
             propertyIterator != entityIterator->value.MemberEnd(); ++propertyIterator) {
            auto property = FindPropertyByName(propertyIterator->name.GetString());
            auto& reader = propertyIterator->value;
            auto type = GetPropertyType(property);

            ReadIf(u8, Uint);
            ReadIf(u16, Uint);
            ReadIf(u32, Uint);
            ReadIf(u64, Uint64);
            ReadIf(s8, Int);
            ReadIf(s16, Int);
            ReadIf(s32, Int);
            ReadIf(s64, Int64);
            ReadIf(StringRef, String);
            ReadIf(float, Double);
            ReadIf(double, Double);
            ReadIf(bool, Bool);
            ReadReferencedIf(Entity, String, CreateEntityFromPath);
            ReadReferencedIf(Type, String, GetTypeFromName);
            ReadVec2If(v2f, Double);
            ReadVec3If(v3f, Double);
            ReadVec4If(v4f, Double);
            ReadVec2If(v2i, Int);
            ReadVec3If(v3i, Int);
            ReadVec4If(v4i, Int);
        }
    }
}

static void LoadHandler(Entity entity) {
    if(HasStreamedPersistancePoint(entity)) {
        auto stream = GetPersistancePointStream(entity);
        if(strcmp(GetStreamMimeType(stream), "application/json") == 0) {
            DeserializeJson(entity, stream);
        }
    }
}

static void SaveHandler(Entity entity) {
    if(HasStreamedPersistancePoint(entity)) {
        auto stream = GetPersistancePointStream(entity);
        if(strcmp(GetStreamMimeType(stream), "application/json") == 0) {
            SerializeJson(entity, stream);
        }
    }
}

static bool ServiceStart() {
    SubscribeLoadStarted(LoadHandler);
    SubscribeSaveStarted(SaveHandler);

    RegisterFileType(".json", "application/json");

    return true;
}

static bool ServiceStop() {
    UnsubscribeLoadStarted(LoadHandler);
    UnsubscribeSaveStarted(SaveHandler);

    return true;
}