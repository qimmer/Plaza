//
// Created by Kim Johannsen on 20/02/2018.
//

#include "JsonPersistance.h"
#include <Core/Node.h>
#include <Core/Types.h>
#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>
#include <File/FileStream.h>
#include <sstream>
#include <cmath>
#include <Core/Debug.h>

#define RAPIDJSON_ASSERT(x) Assert(0, x)

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include <EASTL/string.h>

using namespace eastl;

#define WriteIf(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE) {\
        writer.JSONWRITEFUNC(*(TYPE*)valueData);\
    } else

#define ReadIf(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE) {\
        *(TYPE*)valueData = reader.Get ## JSONREADFUNC ();\
    }  else

#define ReadVec2If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE && reader.GetType() == rapidjson::kObjectType) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        *(TYPE*)valueData = v;\
    } else

#define ReadVec3If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE && reader.GetType() == rapidjson::kObjectType) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        *(TYPE*)valueData = v;\
    } else

#define ReadVec4If(TYPE, JSONREADFUNC)\
    if(type == TypeOf_ ## TYPE && reader.GetType() == rapidjson::kObjectType) {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        v.w = reader.GetObject()["w"].Get ## JSONREADFUNC ();\
        *(TYPE*)valueData = v;\
    } else

#define WriteVec2If(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE) {\
        auto v = *(TYPE*)valueData;\
        writer.StartObject();\
        writer.String("x");\
        writer.JSONWRITEFUNC(v.x);\
        writer.String("y");\
        writer.JSONWRITEFUNC(v.y);\
        writer.EndObject();\
    } else

#define WriteVec3If(TYPE, JSONWRITEFUNC)\
    if(type == TypeOf_ ## TYPE) {\
        auto v = *(TYPE*)valueData;\
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
    if(type == TypeOf_ ## TYPE) {\
        auto v = *(TYPE*)valueData;\
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

static StringRef TryGetEntityPath(Entity entity) {
    if(!IsEntityValid(entity) || !HasComponent(entity, ComponentOf_Node())) {
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

static bool SerializeNode(Entity parent, Entity root, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
    writer.StartObject();
    for_children(child, parent) {
        writer.String(GetName(child));
        writer.StartObject();

        for_entity(property, propertyData, Property) {
            auto component = GetPropertyComponent(property);

            if(!HasComponent(child, component)
               || (parent == root && (component == ComponentOf_PersistancePoint()))) {
                continue;
            }

            if(property == PropertyOf_Name() || property == PropertyOf_Parent()) continue;

            char valueData[64];
            auto type = GetPropertyType(property);
            GetPropertyValue(property, parent, valueData);

            writer.String(GetName(property));

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
            WriteIf(Type, Uint)
            WriteVec2If(v2f, Double)
            WriteVec3If(v3f, Double)
            WriteVec4If(v4f, Double)
            WriteVec2If(v2i, Int)
            WriteVec3If(v3i, Int)
            WriteVec4If(v4i, Int)
            WriteVec4If(rgba8, Int)

            // Special Case, as entity can be an object if part of this tree, or a path string as reference to outside this tree
            if(type == TypeOf_Entity) {
                auto entity = *(Entity*)valueData;
                if(HasComponent(entity, ComponentOf_PersistancePoint())) {
                    writer.String(GetStreamPath(entity));
                } else if (!IsEntityDecendant(entity, root)) {
                    writer.String(GetEntityPath(entity));
                } else {
                    SerializeNode(entity, root, writer);
                }
            } else {
                Log(parent, LogSeverity_Error, "Unsupported type when serializing property '%s': %s", GetName(property), GetTypeName(type));
                writer.Null();
            }
        }

        writer.String("$components");

        std::stringstream componentsStream;
        for_entity(component, componentData, Component) {
            if(HasComponent(parent, component)) {
                componentsStream << GetName(component) << ";";
            }
        }
        auto components = componentsStream.str();
        if(components.c_str()[components.length()] == ';') components.pop_back();
        writer.String(components.c_str());

        writer.EndObject();
    }
    writer.EndObject();
}

static bool DeserializeNode(Entity parent, Entity root, rapidjson::Value& value) {
    string componentList;

    for (auto propertyIterator = value.MemberBegin();
         propertyIterator != value.MemberEnd(); ++propertyIterator)
    {
        auto propertyName = propertyIterator->name.GetString();

        if(strcmp(propertyName, "$components") == 0 && propertyIterator->value.IsString()) {
            componentList = propertyIterator->value.GetString();
            continue;
        }

        for_entity(property, propertyData, Property) {
            if(strcmp(propertyName, GetName(property)) == 0) break;
        }

        if(!IsEntityValid(property)) {
            Log(root, LogSeverity_Warning, "Unknown property when deserializing '%s': %s", GetStreamPath(root), propertyName);
            continue;
        }

        auto& reader = propertyIterator->value;
        auto type = GetPropertyType(property);
        auto jsonType =propertyIterator->value.GetType();
        auto typeName = GetTypeName(type);
        char valueData[64];

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
        ReadIf(Type, Uint)
        ReadVec2If(v2f, Double)
        ReadVec3If(v3f, Double)
        ReadVec4If(v4f, Double)
        ReadVec2If(v2i, Int)
        ReadVec3If(v3i, Int)
        ReadVec4If(v4i, Int)
        ReadVec4If(rgba8, Int)
        {
            Log(root, LogSeverity_Error, "Unsupported type when deserializing property '%s': %s", GetName(property), typeName);
        }

        SetPropertyValue(property, parent, valueData);
    }
    string element;
    size_t start = 0;
    while(start < componentList.length()) {
        auto nextSep = componentList.find(';', start);
        if(nextSep == string::npos) nextSep = componentList.length();
        auto componentName = componentList.substr(start, nextSep - start);
        start = nextSep + 1;

        for_entity(component, componentData, Component) {
            if(strcmp(componentName.c_str(), GetName(component)) == 0) break;
        }

        if(!IsEntityValid(component)) {
            Log(root, LogSeverity_Warning, "Unknown component in JSON: %s", componentName.c_str());
        } else {
            AddComponent(parent, component);
        }
    }
}

static bool Serialize(Entity persistancePoint) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    auto result = SerializeNode(persistancePoint, persistancePoint, writer);

    writer.Flush();

    Assert(persistancePoint, StreamOpen(persistancePoint, StreamMode_Write));
    Assert(persistancePoint, StreamWrite(persistancePoint, buffer.GetLength(), buffer.GetString()));
    StreamClose(persistancePoint);

    return result;
}

static bool Deserialize(Entity persistancePoint) {
    Assert(persistancePoint, StreamOpen(persistancePoint, StreamMode_Read));
    Assert(persistancePoint, StreamSeek(persistancePoint, StreamSeek_End));
    auto size = StreamTell(persistancePoint);
    char *data = (char*)malloc(size + 1);
	Assert(persistancePoint, data);
    Assert(persistancePoint, StreamSeek(persistancePoint, 0));
    Assert(persistancePoint, StreamRead(persistancePoint, size, data));
    StreamClose(persistancePoint);
    data[size] = '\0';

    rapidjson::Document document;
    document.Parse(data, size);

    auto result = DeserializeNode(persistancePoint, persistancePoint, document);

    free(data);

    return result;
}

BeginUnit(JsonPersistance)
    RegisterSerializer(JsonSerializer, "application/json")
    RegisterFileType("json", "application/json")
EndUnit()
