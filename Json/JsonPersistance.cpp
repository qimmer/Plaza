//
// Created by Kim Johannsen on 20/02/2018.
//

#include "JsonPersistance.h"
#include <Core/Types.h>
#include <Foundation/Stream.h>
#include <Json/NativeUtils.h>
#include <Foundation/PersistancePoint.h>
#include <File/FileStream.h>
#include <sstream>
#include <cmath>
#include <Core/Debug.h>

#define RAPIDJSON_ASSERT(x) Assert(0, x)

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include <EASTL/string.h>
#include <EASTL/fixed_vector.h>
#include <Foundation/NativeUtils.h>
#include <Core/Identification.h>
#include <Core/Math.h>

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

static bool SerializeNode(Entity parent, Entity root, rapidjson::Writer<rapidjson::StringBuffer>& writer, s16 includeChildLevels, s16 includeReferenceLevels);

static bool SerializeValue(Entity entity, Entity property, Entity root, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
    char entityPath[PathMax];
    char valueData[64];
    auto type = GetPropertyType(property);
    GetPropertyValue(property, entity, valueData);

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
                CalculateEntityPath(entityPath, PathMax, entity, true);
                writer.String(GetEntityRelativePath(entityPath, root, true));
            } else {
                writer.Null();
            }
        }

            break;
        default:
            Log(entity, LogSeverity_Error, "Unsupported type when serializing property '%s': %s", GetName(property), GetTypeName(type));
            writer.Null();
            return false;
    }

    return true;
}

static bool SerializeNode(Entity parent, Entity root, rapidjson::Writer<rapidjson::StringBuffer>& writer, s16 includeChildLevels, s16 includeReferenceLevels) {
    bool result = true;

    writer.StartObject();

    for_entity(component, componentData, Component) {
        if(!HasComponent(parent, component)
           || (parent == root && (component == ComponentOf_PersistancePoint()))
           || component == ComponentOf_Ownership()) {
            continue;
        }

        auto numProperties = GetNumProperties(component);
        auto properties = GetProperties(component);
        for(auto pi = 0; pi < numProperties; ++pi) {
            auto property = properties[pi];
            auto name = GetName(property);

            auto flags = GetPropertyFlags(property);
            if(flags & PropertyFlag_Transient || flags & PropertyFlag_ReadOnly) continue;

            switch(GetPropertyKind(property)) {
                case PropertyKind_String:
                case PropertyKind_Value:
                    writer.String(name);

                    if(includeReferenceLevels > 0 && GetPropertyType(property) == TypeOf_Entity) {
                        Entity child = 0;
                        GetPropertyValue(property, parent, &child);

                        if(IsEntityValid(child)) {
                            result &= SerializeNode(child, root, writer, Max(includeChildLevels - 1, 0), Max(includeReferenceLevels - 1, 0));
                        } else {
                            writer.Null();
                        }
                    } else {
                        result &= SerializeValue(parent, property, root, writer);
                    }

                    break;
                case PropertyKind_Child:
                {
                    Entity child = 0;
                    GetPropertyValue(property, parent, &child);
                    if(IsEntityValid(child) && includeChildLevels > 0) {
                        writer.String(GetName(property));
                        result &= SerializeNode(child, root, writer, Max(includeChildLevels - 1, 0), Max(includeReferenceLevels - 1, 0));
                    }
                }
                    break;
                case PropertyKind_Array:
                {
                    if(includeChildLevels > 0) {
                        writer.String(GetName(property));

                        auto count = GetArrayPropertyCount(property, parent);
                        writer.StartArray();
                        for(auto i = 0; i < count; ++i) {
                            auto child = GetArrayPropertyElement(property, parent, i);
                            if(IsEntityValid(child)) {
                                result &= SerializeNode(child, root, writer, Max(includeChildLevels - 1, 0), Max(includeReferenceLevels - 1, 0));
                            } else {
                                writer.Null();
                            }

                        }
                        writer.EndArray();
                    }
                }
                    break;
                default:
                    Log(property, LogSeverity_Error, "Unknown property kind: %d", GetPropertyKind(property));
                    break;
            }
        }
    }

    {
        writer.String("$components");
        writer.StartArray();
        for_entity(component, componentData, Component) {
            if(HasComponent(parent, component)) {
                writer.String(GetName(component));
            }
        }
        writer.EndArray();

        //if(parent == root) {
            char path[2048];
            CalculateEntityPath(path, sizeof(path), parent, true);

            writer.String("$path");
            writer.String(path);
        //}
    }

    writer.EndObject();

    return result;
}

static bool DeserializeValue(Entity parent, Entity property, Entity root, const rapidjson::Value& reader) {
    auto type = GetPropertyType(property);
    auto jsonType = reader.GetType();
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
            if(!reader.IsString()) {
                Log(root, LogSeverity_Error, "Error parsing property '%s': Types should be noted by it's name.", GetName(property));
                break;
            }
            *(Type*)valueData = FindType(reader.GetString());
            break;
        case TypeOf_Entity:
        {
            if(reader.IsNull()) {
                *(Entity*)valueData = 0;
                break;
            } else if(!reader.IsString()) {
                Log(root, LogSeverity_Error, "Error parsing property '%s': Entity references should be noted by a path string in JSON. Use $children to note entity hierarchy data.", GetName(property));
                break;
            }

            char absolutePath[PathMax];
            auto path = reader.GetString();

            if(path[0] == '/') {
                snprintf(absolutePath, PathMax, "%s", path);
            } else {
                char rootPath[PathMax];
                CalculateEntityPath(rootPath, PathMax, root, true);

                snprintf(absolutePath, PathMax, "%s/%s", rootPath, path);
            }

            LoadEntityPath(path); // Ensure referred entities are loaded, if they persist in any unloaded persistance points
            auto entity = FindEntityByPath(path);
            *(Entity*)valueData = entity;
            if(!IsEntityValid(entity)) {
                Log(root, LogSeverity_Error, "Could not find entity by path: %s", path);
            }
        }
            break;
        default:
            Log(root, LogSeverity_Error, "Unsupported type when deserializing property '%s': %s", GetName(property), typeName);
            return false;
            break;
    }

    SetPropertyValue(property, parent, valueData);

    return true;
}

static StringRef GetJsonTypeName(const rapidjson::Value& value) {
    StringRef jsonType = NULL;
    switch(value.GetType()) {
        case rapidjson::kNullType:      //!< null
            jsonType = "Null";
            break;
        case rapidjson::kFalseType:     //!< false
            jsonType = "False";
            break;
        case rapidjson::kTrueType:      //!< true
            jsonType = "True";
            break;
        case rapidjson::kObjectType:    //!< object
            jsonType = "Object";
            break;
        case rapidjson::kArrayType:     //!< array
            jsonType = "Array";
            break;
        case rapidjson::kStringType:    //!< string
            jsonType = "String";
            break;
        case rapidjson::kNumberType:     //!< number
            jsonType = "Number";
            break;
        default:
            jsonType = "Unknown";
            break;
    }
    return jsonType;
}
static bool DeserializeNode(Entity parent, Entity root, const rapidjson::Value& value, bool onlyHierarchy) {
    bool result = true;

    if(!value.IsObject()) {
        Log(parent, LogSeverity_Error, "Json value for %s is not an object, but a %s. Skipping this node.", GetName(parent), GetJsonTypeName(value));
        return false;
    }

    auto components = value.FindMember("$components");

    for (auto propertyIterator = value.MemberBegin();
         propertyIterator != value.MemberEnd(); ++propertyIterator)
    {
        auto propertyName = propertyIterator->name.GetString();

        if(propertyName[0] == '$') {
            continue;
        }

        for_entity(property, propertyData, Property) {
            if(strcmp(propertyName, GetName(property)) == 0) break;
        }

        if(!IsEntityValid(property)) {
            Log(root, LogSeverity_Warning, "Unknown property when deserializing '%s': %s", GetStreamPath(root), propertyName);
            continue;
        }

        if(property == PropertyOf_Owner()) continue;

        auto propertyKind = GetPropertyKind(property);
        auto& reader = propertyIterator->value;
        switch(propertyKind) {
            case PropertyKind_String:
            case PropertyKind_Value:
                if(!onlyHierarchy) {
                    result &= DeserializeValue(parent, property, root, reader);
                }
                break;
            case PropertyKind_Child:
            {
                AddComponent(parent, GetOwner(property));
                Entity child = 0;
                GetPropertyValue(property, parent, &child);
                result &= DeserializeNode(child, root, reader, onlyHierarchy);
            }
            break;
            case PropertyKind_Array:
            {
                if(!reader.IsArray()) {
                    Log(parent, LogSeverity_Error, "Property %s is an array property, but JSON value is %s. Skipping this property", GetName(property), GetJsonTypeName(reader));
                    continue;
                }
                AddComponent(parent, GetOwner(property));
                auto count = reader.Size();
                auto existingCount = GetArrayPropertyCount(property, parent);
                for(auto i = 0; i < count; ++i) {
                    auto& element = reader[i];

                    if(i >= existingCount) {
                        AddArrayPropertyElement(property, parent); // Add missing element
                    }

                    auto child = GetArrayPropertyElement(property, parent, i);
                    DeserializeNode(child, root, element, onlyHierarchy);
                }

                for(auto i = count; i < existingCount; ++i) { // Remove excess elements
                    RemoveArrayPropertyElement(property, parent, count);
                }
            }
            break;
            default:
                Log(root, LogSeverity_Warning, "Unknown property kind: %d", propertyKind);
                break;
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

    return result;
}

API_EXPORT bool DeserializeJsonFromString(Entity stream, Entity entity, StringRef jsonString) {
    rapidjson::Document document;
    auto size = strlen(jsonString);
    auto parseResult = (rapidjson::ParseResult)document.Parse(jsonString, size);

    if(parseResult.IsError()) {
        char snippet[4096];
        snprintf(snippet, 4096, ">> %s <<\n\n%s", jsonString + Min(size, parseResult.Offset()), jsonString);

        Log(stream, LogSeverity_Error, "JSON parse error: %s %s", GetParseError_En(parseResult.Code()), snippet);
        return false;
    }

    auto result = DeserializeNode(stream, entity, document, true) && DeserializeNode(entity, entity, document, false);

    return result;
}

API_EXPORT bool SerializeJson(Entity stream, Entity entity, s16 includeChildLevels, s16 includeReferenceLevels) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    auto result = SerializeNode(entity, entity, writer, includeChildLevels, includeReferenceLevels);

    writer.Flush();

    bool streamWasOpen = IsStreamOpen(stream);
    if(!streamWasOpen) {
        Assert(stream, StreamOpen(stream, StreamMode_Write));
    }

    Assert(stream, StreamWrite(stream, buffer.GetLength(), buffer.GetString()));

    if(!streamWasOpen) {
        StreamClose(stream);
    }

    return result;
}

API_EXPORT bool DeserializeJson(Entity stream, Entity entity) {
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
    Assert(stream, StreamRead(stream, size, data));

    if(!streamWasOpen) {
        StreamClose(stream);
    }

    data[size] = '\0';

    rapidjson::Document document;
    auto parseResult = (rapidjson::ParseResult)document.Parse(data, size);

    if(parseResult.IsError()) {
        char snippet[4096];
        snprintf(snippet, 4096, ">> %s <<\n\n%s", data + Min(size, parseResult.Offset()), data);

        Log(stream, LogSeverity_Error, "JSON parse error: %s %s", GetParseError_En(parseResult.Code()), snippet);
        return false;
    }

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

	ModuleData(
		{
			"FileTypes": [
				{ "FileTypeExtension": ".json", "FileTypeMimeType" : "application/json" }
			]
		}
	);
EndUnit()
