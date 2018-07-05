//
// Created by Kim Johannsen on 20/02/2018.
//

#include "JsonPersistance.h"
#include <Core/Hierarchy.h>
#include <Core/Types.h>
#include <Core/Type.h>
#include <Core/String.h>
#include <Foundation/Persistance.h>
#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>
#include <File/FileStream.h>
#include <sstream>
#include <cmath>

#define RAPIDJSON_ASSERT(x) Assert(x)

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#define DeclareGetSet(TYPE) typedef TYPE (* TYPE ## Getter)(Entity entity); typedef void(* TYPE ## Setter)(Entity entity, TYPE value);

#define WriteIf(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        writer.JSONWRITEFUNC(((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity));\
    } else

#define ReadIf(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, reader.Get ## JSONREADFUNC ());\
    }  else

#define WriteReferencedIf(TYPE, JSONWRITEFUNC, CONVERTERFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        writer.JSONWRITEFUNC(CONVERTERFUNC(((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity)));\
    } else

#define ReadReferencedIf(TYPE, JSONREADFUNC, CONVERTERFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, CONVERTERFUNC(reader.Get ## JSONREADFUNC ()));\
    } else

#define ReadVec2If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE () && reader.GetType() == rapidjson::kObjectType) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, v);\
    } else

#define ReadVec3If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE () && reader.GetType() == rapidjson::kObjectType) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, v);\
    } else

#define ReadVec4If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE () && reader.GetType() == rapidjson::kObjectType) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        v.w = reader.GetObject()["w"].Get ## JSONREADFUNC ();\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, v);\
    } else

#define ReadMat4x4If(TYPE)\
    if(type == TypeOf_ ## TYPE () && reader.GetType() == rapidjson::kArrayType) {\
        TYPE v;\
        auto arr = reader.GetArray();\
        for(auto i = 0; i < 16; ++i) {\
            ((float*)&v.x.x)[i] = arr[i].GetDouble();\
        }\
        ((TYPE ## Setter)GetPropertySetter(property))(persistentEntity, v);\
    } else

#define WriteVec2If(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE ()) {\
        auto v = ((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity);\
        writer.StartObject();\
        writer.String("x");\
        writer.JSONWRITEFUNC(v.x);\
        writer.String("y");\
        writer.JSONWRITEFUNC(v.y);\
        writer.EndObject();\
    } else

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
    } else

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
    } else

#define WriteMat4x4If(TYPE)\
    if(type == TypeOf_ ## TYPE ()) {\
        auto v = ((TYPE ## Getter)GetPropertyGetter(property))(persistentEntity);\
        writer.StartArray();\
        for(auto i = 0; i < 16; ++i) {\
            auto val = ((float*)&v.x.x)[i];\
            if(!std::isfinite(val)) val = 0.0f;\
            writer.Double(val);\
        }\
        writer.EndArray();\
    } else

DeclareGetSet(u8)
DeclareGetSet(u16)
DeclareGetSet(u32)
DeclareGetSet(u64)
DeclareGetSet(s8)
DeclareGetSet(s16)
DeclareGetSet(s32)
DeclareGetSet(s64)
DeclareGetSet(float)
DeclareGetSet(double)
DeclareGetSet(bool)
DeclareGetSet(v2f)
DeclareGetSet(v3f)
DeclareGetSet(v4f)
DeclareGetSet(v2i)
DeclareGetSet(v3i)
DeclareGetSet(v4i)
DeclareGetSet(m3x3f)
DeclareGetSet(m4x4f)
DeclareGetSet(rgba8)
DeclareGetSet(Entity)
DeclareGetSet(Type)
DeclareGetSet(StringRef)

static StringRef TryGetEntityPath(Entity entity) {
    if(!IsEntityValid(entity) || !HasHierarchy(entity)) {
        return "";
    }

    return GetEntityPath(entity);
}

static Entity TryCreateEntityFromPath(StringRef path) {
    if(strlen(path) == 0) {
        return 0;
    }

    return CreateEntityFromPath(path);
}

static bool SerializeJson(Entity persistancePoint) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    for_entity(persistentEntity, Persistance) {
        if(GetEntityPersistancePoint(persistentEntity) != persistancePoint) continue;

        writer.String(GetEntityPath(persistentEntity));
        writer.StartObject();

        for(auto property = GetNextProperty(0); IsPropertyValid(property); property = GetNextProperty(property)) {
            auto componentType = GetPropertyComponent(property);
            if(!HasComponent(persistentEntity, componentType)) {
                continue;
            }

            if(strcmp(GetPropertyName(property), "Name") == 0 ||
                strcmp(GetPropertyName(property), "EntityPersistancePoint") == 0 ||
                strcmp(GetPropertyName(property), "Parent") == 0) continue;

            auto type = GetPropertyType(property);

            writer.String(GetPropertyName(property));

            WriteIf(u8, Uint)
            WriteIf(u16, Uint)
            WriteIf(u32, Uint)
            WriteIf(u64, Uint64)
            WriteIf(s8, Int)
            WriteIf(s16, Int)
            WriteIf(s32, Int)
            WriteIf(s64, Int64)
            WriteIf(StringRef, String)
            WriteIf(float, Double)
            WriteIf(double, Double)
            WriteIf(bool, Bool)
            WriteReferencedIf(Entity, String, TryGetEntityPath)
            WriteReferencedIf(Type, String, GetTypeName)
            WriteVec2If(v2f, Double)
            WriteVec3If(v3f, Double)
            WriteVec4If(v4f, Double)
            WriteVec2If(v2i, Int)
            WriteVec3If(v3i, Int)
            WriteVec4If(v4i, Int)
            WriteVec4If(rgba8, Int)
            WriteMat4x4If(m4x4f)
            {
                Log(LogChannel_Core, LogSeverity_Error, "Unsupported type when serializing property '%s': %s", GetPropertyName(property), GetTypeName(type));
                writer.Null();
            }
        }

        writer.String("$components");

        std::stringstream componentsStream;
        for(auto type = GetNextType(0); IsTypeValid(type); type = GetNextType(type)) {
            if(!IsComponent(type)) continue;

            if(HasComponent(persistentEntity, type)) {
                componentsStream << GetTypeName(type) << ";";
            }
        }
        auto components = componentsStream.str();
        if(components.c_str()[components.length()] == ';') components.pop_back();
        writer.String(components.c_str());

        writer.EndObject();
    }
    writer.EndObject();

    writer.Flush();

    Assert(StreamOpen(persistancePoint, StreamMode_Write));
    Assert(StreamWrite(persistancePoint, buffer.GetLength(), buffer.GetString()));
    StreamClose(persistancePoint);

    return true;
}

static bool DeserializeJson(Entity persistancePoint) {
    Assert(StreamOpen(persistancePoint, StreamMode_Read));
    Assert(StreamSeek(persistancePoint, StreamSeek_End));
    auto size = StreamTell(persistancePoint);
    char *data = (char*)malloc(size + 1);
	Assert(data);
    Assert(StreamSeek(persistancePoint, 0));
    Assert(StreamRead(persistancePoint, size, data));
    StreamClose(persistancePoint);
    data[size] = '\0';

    rapidjson::Document document;
    document.Parse(data, size);

    for (auto entityIterator = document.MemberBegin();
         entityIterator != document.MemberEnd(); ++entityIterator)
    {
        auto entityPath = entityIterator->name.GetString();
        auto persistentEntity = CreateEntityFromPath(entityPath);
        SetEntityPersistancePoint(persistentEntity, persistancePoint);

        String componentList;

        for (auto propertyIterator = entityIterator->value.MemberBegin();
             propertyIterator != entityIterator->value.MemberEnd(); ++propertyIterator) {
            auto propertyName = propertyIterator->name.GetString();

            if(strcmp(propertyName, "$components") == 0 && propertyIterator->value.IsString()) {
                componentList = propertyIterator->value.GetString();
                continue;
            }

            auto property = FindPropertyByName(propertyName);
            if(!IsPropertyValid(property)) {
                Log(LogChannel_Core, LogSeverity_Warning, "Unknown property when deserializing '%s': %s", GetStreamPath(persistentEntity), propertyName);
                continue;
            }
            auto& reader = propertyIterator->value;
            auto type = GetPropertyType(property);
            auto jsonType =propertyIterator->value.GetType();
            auto typeName = GetTypeName(type);

            ReadIf(u8, Uint)
            ReadIf(u16, Uint)
            ReadIf(u32, Uint)
            ReadIf(u64, Uint64)
            ReadIf(s8, Int)
            ReadIf(s16, Int)
            ReadIf(s32, Int)
            ReadIf(s64, Int64)
            ReadIf(StringRef, String)
            ReadIf(float, Double)
            ReadIf(double, Double)
            ReadIf(bool, Bool)
            ReadReferencedIf(Entity, String, TryCreateEntityFromPath)
            ReadReferencedIf(Type, String, FindTypeByName)
            ReadVec2If(v2f, Double)
            ReadVec3If(v3f, Double)
            ReadVec4If(v4f, Double)
            ReadVec2If(v2i, Int)
            ReadVec3If(v3i, Int)
            ReadVec4If(v4i, Int)
            ReadVec4If(rgba8, Int)
            ReadMat4x4If(m4x4f)
            {
                Log(LogChannel_Core, LogSeverity_Error, "Unsupported type when deserializing property '%s': %s", GetPropertyName(property), GetTypeName(property));
            }
        }

        String element;
        size_t start = 0;
        while(start < componentList.length()) {
            auto nextSep = componentList.find(';', start);
            if(nextSep == String::npos) nextSep = componentList.length();
            auto componentName = componentList.substr(start, nextSep - start);
            start = nextSep + 1;

            auto componentType = FindTypeByName(componentName.c_str());
            if(!IsTypeValid(componentType)) {
                Log(LogChannel_Core, LogSeverity_Warning, "Unknown component type in JSON: %s", componentName.c_str());
            } else {
                AddComponent(persistentEntity, componentType);
            }
        }
    }

    free(data);

    return true;
}

static void OnServiceStart(Service service) {
    Serializer s {
        SerializeJson,
        DeserializeJson
    };
    AddFileType(".json", "application/json");
    AddSerializer("application/json", &s);
}

static void OnServiceStop(Service service){
    RemoveFileType(".json");
    RemoveSerializer("application/json");
}

DefineService(JsonPersistance)
    Subscribe(JsonPersistanceStarted, OnServiceStart)
    Subscribe(JsonPersistanceStopped, OnServiceStop)
EndService()
