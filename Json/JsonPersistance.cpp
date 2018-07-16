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
#include <EASTL/fixed_vector.h>
#include <Foundation/NativeUtils.h>

using namespace eastl;

#define WriteIf(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
        writer.JSONWRITEFUNC(*(TYPE*)valueData);\
        break;

#define ReadIf(TYPE, JSONREADFUNC)\
    case TypeOf_ ## TYPE:\
        *(TYPE*)valueData = reader.Get ## JSONREADFUNC ();\
        break;

#define ReadVec2If(TYPE, JSONREADFUNC)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        *(TYPE*)valueData = v;\
        }\
        break;

#define ReadVec3If(TYPE, JSONREADFUNC)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        *(TYPE*)valueData = v;\
        }\
        break;

#define ReadVec4If(TYPE, JSONREADFUNC)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
        v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        v.w = reader.GetObject()["w"].Get ## JSONREADFUNC ();\
        *(TYPE*)valueData = v;\
        }\
        break;

#define WriteVec2If(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
        {\
            auto v2 = *(TYPE*)valueData;\
            writer.StartObject();\
            writer.String("x");\
            writer.JSONWRITEFUNC(v2.x);\
            writer.String("y");\
            writer.JSONWRITEFUNC(v2.y);\
            writer.EndObject();\
        }\
        break;

#define WriteVec3If(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
{\
        auto v3 = *(TYPE*)valueData;\
        writer.StartObject();\
        writer.String("x");\
        writer.JSONWRITEFUNC(v3.x);\
        writer.String("y");\
        writer.JSONWRITEFUNC(v3.y);\
        writer.String("z");\
        writer.JSONWRITEFUNC(v3.z);\
        writer.EndObject();\
        }\
        break;

#define WriteVec4If(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
{\
        auto v4 = *(TYPE*)valueData;\
        writer.StartObject();\
        writer.String("x");\
        writer.JSONWRITEFUNC(v4.x);\
        writer.String("y");\
        writer.JSONWRITEFUNC(v4.y);\
        writer.String("z");\
        writer.JSONWRITEFUNC(v4.z);\
        writer.String("w");\
        writer.JSONWRITEFUNC(v4.w);\
        writer.EndObject();\
        }\
        break;

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

    for_entity(property, propertyData, Property) {
        auto component = GetPropertyComponent(property);

        if(!HasComponent(parent, component)
           || (parent == root && (component == ComponentOf_PersistancePoint()))
           || GetPropertyFlags(component) & PropertyFlag_Transient
           || GetPropertyFlags(component) & PropertyFlag_ReadOnly) {
            continue;
        }

        if(property == PropertyOf_Parent()) continue;

        char valueData[64];
        auto type = GetPropertyType(property);
        GetPropertyValue(property, parent, valueData);

        writer.String(GetName(property));

        switch(type) {
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
            WriteVec2If(v2f, Double)
            WriteVec3If(v3f, Double)
            WriteVec4If(v4f, Double)
            WriteVec2If(v2i, Int)
            WriteVec3If(v3i, Int)
            WriteVec4If(v4i, Int)
            WriteVec4If(rgba8, Int)
            case TypeOf_Type:
                writer.String(GetTypeName(*(Type*)valueData));
                break;
            case TypeOf_Entity:
            {
                auto entity = *(Entity*)valueData;
                if(IsEntityValid(entity)) {
                    writer.String(GetEntityRelativePath(entity, root));
                } else {
                    writer.Null();
                }
            }

                break;
            default:
                Log(parent, LogSeverity_Error, "Unsupported type when serializing property '%s': %s", GetName(property), GetTypeName(type));
                writer.Null();
                break;
        }
    }

    writer.String("$components");
    writer.StartArray();
    for_entity(component, componentData, Component) {
        if(HasComponent(parent, component)) {
            writer.String(GetName(component));
        }
    }
    writer.EndArray();

    if(root == parent || !HasComponent(parent, ComponentOf_PersistancePoint())) {
        writer.String("$children");
        writer.StartArray();
        for_children(child, parent) {
            SerializeNode(child, root, writer);
        }
        writer.EndArray();
    }

    writer.EndObject();

    return true;
}

static bool DeserializeNode(Entity parent, Entity root, const rapidjson::Value& value, bool onlyHierarchy) {
    auto components = value.FindMember("$components");
    auto children = value.FindMember("$children");

    if(!onlyHierarchy) {
        for (auto propertyIterator = value.MemberBegin();
             propertyIterator != value.MemberEnd(); ++propertyIterator)
        {
            auto propertyName = propertyIterator->name.GetString();

            if(propertyName[0] == '$' || strcmp(propertyName, "Parent") == 0 || strcmp(propertyName, "Name") == 0) {
                continue;
            }

            for_entity(property, propertyData, Property) {
                if(strcmp(propertyName, GetName(property)) == 0) break;
            }

            if(!IsEntityValid(property)) {
                Log(root, LogSeverity_Warning, "Unknown property when deserializing '%s': %s", GetStreamPath(root), propertyName);
                continue;
            }

            if(property == PropertyOf_Parent()) continue;

            auto& reader = propertyIterator->value;
            auto type = GetPropertyType(property);
            auto jsonType =propertyIterator->value.GetType();
            auto typeName = GetTypeName(type);
            char valueData[64];

            switch(type) {
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
                ReadVec2If(v2f, Double)
                ReadVec3If(v3f, Double)
                ReadVec4If(v4f, Double)
                ReadVec2If(v2i, Int)
                ReadVec3If(v3i, Int)
                ReadVec4If(v4i, Int)
                ReadVec4If(rgba8, Int)
                case TypeOf_Type:
                    if(!propertyIterator->value.IsString()) {
                        Log(root, LogSeverity_Error, "Error parsing property '%s': Types should be noted by it's name.", propertyName);
                        break;
                    }
                    *(Type*)valueData = FindType(reader.GetString());
                    break;
                case TypeOf_Entity:
                {
                    if(propertyIterator->value.IsNull()) {
                        *(Entity*)valueData = 0;
                        break;
                    } else if(!propertyIterator->value.IsString()) {
                        Log(root, LogSeverity_Error, "Error parsing property '%s': Entity references should be noted by a path string in JSON. Use $children to note entity hierarchy data.", propertyName);
                        break;
                    }

                    char absolutePath[PATH_MAX];
                    auto path = propertyIterator->value.GetString();

                    if(path[0] == '/') {
                        snprintf(absolutePath, PATH_MAX, "%s", path);
                    } else {
                        snprintf(absolutePath, PATH_MAX, "%s/%s", GetEntityPath(root), path);
                    }

                    *(Entity*)valueData = CreateEntityFromPath(absolutePath);
                }
                    break;
                default:
                    Log(root, LogSeverity_Error, "Unsupported type when deserializing property '%s': %s", GetName(property), typeName);
            }

            SetPropertyValue(property, parent, valueData);
        }
    }

    if(components != value.MemberEnd() && components->value.IsArray()) {
        for(auto arrayIt = components->value.Begin(); arrayIt != components->value.End(); ++arrayIt) {
            if(arrayIt->IsString()) {
                auto componentName = arrayIt->GetString();
                for_entity(component, componentData, Component) {
                    if(strcmp(componentName, GetName(component)) == 0) break;
                }

                if(!IsEntityValid(component)) {
                    Log(root, LogSeverity_Warning, "Unknown component in JSON: %s", componentName);
                } else {
                    AddComponent(parent, component);
                }
            }
        }
    }

    if(children != value.MemberEnd() && children->value.IsArray()) {
        for(auto arrayIt = children->value.Begin(); arrayIt != children->value.End(); ++arrayIt) {

            auto name = arrayIt->FindMember("Name");

            if(name == value.MemberEnd() || !name->value.IsString()) {
                Log(root, LogSeverity_Error, "JSON child to '%s' needs a Name property (string).", GetEntityPath(parent));
                continue;
            }

            auto child = CreateEntityFromName(parent, name->value.GetString());

            if(!DeserializeNode(child, root, *arrayIt, onlyHierarchy)) {
                return false;
            }
        }
    }

    return true;
}

API_EXPORT bool SerializeJson(Entity stream, Entity entity) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    auto result = SerializeNode(entity, entity, writer);

    writer.Flush();

    Assert(stream, StreamOpen(stream, StreamMode_Write));
    Assert(stream, StreamWrite(stream, buffer.GetLength(), buffer.GetString()));
    StreamClose(stream);

    return result;
}

API_EXPORT bool DeserializeJson(Entity stream, Entity entity) {
    Assert(stream, StreamOpen(stream, StreamMode_Read));
    Assert(stream, StreamSeek(stream, StreamSeek_End));
    auto size = StreamTell(stream);
    char *data = (char*)malloc(size + 1);
    Assert(stream, data);
    Assert(stream, StreamSeek(stream, 0));
    Assert(stream, StreamRead(stream, size, data));
    StreamClose(stream);
    data[size] = '\0';

    rapidjson::Document document;
    document.Parse(data, size);

    auto result = DeserializeNode(entity, entity, document, true) && DeserializeNode(entity, entity, document, false);

    free(data);

    return result;
}

static bool Serialize(Entity persistancePoint) {
    return SerializeJson(persistancePoint, persistancePoint);
}

static bool Deserialize(Entity persistancePoint) {
    return DeserializeJson(persistancePoint, persistancePoint);
}

BeginUnit(JsonPersistance)
    RegisterSerializer(JsonSerializer, "application/json")
    RegisterFileType(".json", "application/json", 0)
EndUnit()
