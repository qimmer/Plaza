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
#include "rapidjson/pointer.h"
#include "rapidjson/error/en.h"
#include "rapidjson/filewritestream.h"

#include <EASTL/string.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/fixed_hash_map.h>
#include <Foundation/NativeUtils.h>
#include <Core/Identification.h>
#include <Core/Math.h>

#include <unordered_map>
#include <string>
#include <Core/Std.h>
#include <Foundation/SerializationSettings.h>
#include <Foundation/FoundationModule.h>

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

static bool SerializeNode(Entity parent, Entity root, rapidjson::Writer<rapidjson::StringBuffer>& writer, s16 includeChildLevels, s16 includeReferenceLevels, bool includePersistedChildren, bool includeNativeEntities);
static bool DeserializeValue(Entity parent, Entity property, rapidjson::Value& entityMap, rapidjson::Document& document, const rapidjson::Value& reader);
static Entity ResolveEntity(StringRef uuid, rapidjson::Value& entityMap, rapidjson::Document& document);

static bool SerializeValue(Entity entity, Entity property, Entity root, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
    char valueData[64];
    auto type = GetPropertyType(property);
    GetPropertyValue(property, entity, valueData);

    switch(type) {
        WriteIf(u8, Uint)
        WriteIf(u16, Uint)
        WriteIf(u32, Uint)
        WriteIf(u64, Uint64)
        WriteIf(Date, Uint64)
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
                writer.String(GetUuid(entity));
            } else {
                writer.Null();
            }
        }

            break;
        default:
            Log(entity, LogSeverity_Warning, "Unsupported type when serializing property '%s': %s", GetName(property), GetTypeName(type));
            writer.Null();
    }

    return true;
}

static bool SerializeNode(Entity parent, Entity root, StringRef parentProperty, rapidjson::Writer<rapidjson::StringBuffer>& writer, s16 includeChildLevels, s16 includeReferenceLevels, bool includePersistedChildren, bool includeNativeEntities) {
    bool result = true;

    // Only serialize if max child level has not been reached
    if(includeChildLevels > 0 && (includeNativeEntities || !GetUniqueEntityName(parent))) {
        writer.StartObject();
        {
            writer.String("$components");
            writer.StartArray();
            for_entity(component, componentData, Component) {
                if(HasComponent(parent, component)) {
                    writer.String(GetUuid(component));
                }
            }
            writer.EndArray();
        }

        writer.String("$owner");
        writer.String(GetUuid(GetOwner(parent)));

        {
            writer.String("$dependencies");
            writer.StartArray();
            for_entity(component, componentData, Component) {
                if (!HasComponent(parent, component) || component == ComponentOf_Ownership()) {
                    continue;
                }

                auto numProperties = GetNumProperties(component);
                auto properties = GetProperties(component);
                for (auto pi = 0; pi < numProperties; ++pi) {
                    auto property = properties[pi];
                    auto name = GetName(property);

                    if(GetPropertyKind(property) != PropertyKind_Value
                    || GetPropertyType(property) != TypeOf_Entity) {
                        continue;
                    }

                    Entity ref = 0;
                    if(GetPropertyValue(property, parent, &ref) && IsEntityValid(ref)) {
                        Entity owner = ref;
                        while(IsEntityValid(owner)) {
                            if(HasComponent(owner, ComponentOf_PersistancePoint())) {
                                char path[512];
                                CalculateEntityPath(path, sizeof(path), owner, true);

                                writer.StartObject();
                                writer.String("EntityPath");
                                writer.String(path);
                                writer.String("StreamPath");
                                writer.String(GetStreamPath(owner));
                                writer.String("Property");
                                writer.String(GetName(property));
                                writer.EndObject();
                            }

                            owner = GetOwner(owner);
                        }
                    }
                }
            }
            writer.EndArray();
        }

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

                auto kind = GetPropertyKind(property);

                // Eventually skip child and array entities if they are persisted elsewhere
                if(!includePersistedChildren && HasComponent(parent, ComponentOf_PersistancePoint()) && kind != PropertyKind_Value && parent != root) {
                    continue;
                }

                switch(kind) {
                    case PropertyKind_Value:
                        writer.String(name);

                        if(includeReferenceLevels > 0 && GetPropertyType(property) == TypeOf_Entity) {
                            Entity child = 0;
                            GetPropertyValue(property, parent, &child);

                            if(IsEntityValid(child)) {
                                result &= SerializeNode(child, root, name, writer, Max(includeChildLevels - 1, 0), Max(includeReferenceLevels - 1, 0), includePersistedChildren, includeNativeEntities);
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
                        if(IsEntityValid(child)) {
                            writer.String(GetName(property));
                            result &= SerializeNode(child, root, name, writer, Max(includeChildLevels - 1, 0), Max(includeReferenceLevels - 1, 0), includePersistedChildren, includeNativeEntities);
                        }
                    }
                        break;
                    case PropertyKind_Array:
                    {
                        writer.String(GetName(property));

                        auto count = GetArrayPropertyCount(property, parent);
                        writer.StartArray();
                        for(auto i = 0; i < count; ++i) {
                            auto child = GetArrayPropertyElement(property, parent, i);
                            if(IsEntityValid(child)) {
                                result &= SerializeNode(child, root, name, writer, Max(includeChildLevels - 1, 0), Max(includeReferenceLevels - 1, 0), includePersistedChildren, includeNativeEntities);
                            } else {
                                writer.Null();
                            }

                        }
                        writer.EndArray();
                    }
                        break;
                    default:
                        Log(property, LogSeverity_Error, "Unknown property kind: %d", GetPropertyKind(property));
                        break;
                }
            }
        }

        writer.EndObject();
    } else {
        writer.String(GetUuid(parent));
    }

    return result;
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

static void DeserializeCodeModel(rapidjson::Value& entityMap, rapidjson::Document& document) {
    for(auto it = entityMap.MemberBegin(); it != entityMap.MemberEnd(); ++it) {
        if(it->value.HasMember("Components") && it->value.HasMember("Uuid")) {
            auto uuid = ToLower(it->value["Uuid"].GetString());
            auto name = it->value["Name"].GetString();

            auto module = FindEntityByUuid(uuid);
            if(!IsEntityValid(module)) {
                module = AddModules(GetModuleRoot());
                SetUuid(module, uuid);
                SetName(module, name);
            }
        }
    }

    for(auto it = entityMap.MemberBegin(); it != entityMap.MemberEnd(); ++it) {
        if(it->value.HasMember("Properties") && it->value.HasMember("Uuid")) {
            auto uuid = ToLower(it->value["Uuid"].GetString());
            auto name = it->value["Name"].GetString();
            auto ownerUuid = ToLower(it->value["$owner"].GetString());

            auto module = FindEntityByUuid(ownerUuid);
            auto component = FindEntityByUuid(uuid);
            if(!IsEntityValid(component)) {
                component = AddComponents(module);
                SetUuid(component, uuid);
                SetName(component, name);
            }
        }
    }

    for(auto it = entityMap.MemberBegin(); it != entityMap.MemberEnd(); ++it) {
        if(it->value.HasMember("PropertyType") && it->value.HasMember("Uuid") && it->value.HasMember("Name")) {
            auto uuid = ToLower(it->value["Uuid"].GetString());
            auto name = it->value["Name"].GetString();
            auto ownerUuid = ToLower(it->value["$owner"].GetString());

            auto component = FindEntityByUuid(ownerUuid);

            auto property = FindEntityByUuid(uuid);
            if(!IsEntityValid(property)) {
                property = AddProperties(component);
                SetUuid(property, uuid);
                SetName(property, name);
            }
        }
    }
}

static bool DeserializeEntity(Entity entity, rapidjson::Value& value, rapidjson::Value& entityMap, rapidjson::Document& document) {
    if(!value.IsObject()) {
        Log(0, LogSeverity_Error, "Json value is not an object, but a %s. Skipping this node.", GetJsonTypeName(value));
        return false;
    }

    value.AddMember("$resolved", true, document.GetAllocator());

    auto components = value.FindMember("$components");
    if(components != value.MemberEnd() && components->value.IsArray()) {
        for(auto arrayIt = components->value.Begin(); arrayIt != components->value.End(); ++arrayIt) {
            if(arrayIt->IsString()) {
                auto componentUuid = arrayIt->GetString();
                auto component = ResolveEntity(componentUuid, entityMap, document);

                if(!IsEntityValid(component)) {
                    Log(0, LogSeverity_Warning, "Unknown component in JSON: %s", componentUuid);
                } else {
                    AddComponent(entity, component);
                }
            }
        }
    }

    for (auto propertyIterator = value.MemberBegin();
         propertyIterator != value.MemberEnd(); ++propertyIterator)
    {
        auto propertyName = Intern(propertyIterator->name.GetString());

        if(propertyName[0] == '$') {
            continue;
        }

        auto property = FindEntityByName(ComponentOf_Property(), propertyName);
        auto propertyUuid = GetUuid(property);
        ResolveEntity(propertyUuid, entityMap, document);

        if(!HasComponent(property, ComponentOf_Property())) {
            Log(0, LogSeverity_Warning, "Unknown property when deserializing '%s': %s", GetDebugName(entity), propertyName);
            continue;
        }

        if(property == PropertyOf_Owner() || GetPropertyReadOnly(property)) continue;

        auto component = GetOwner(property);

        auto propertyKind = GetPropertyKind(property);
        auto& reader = propertyIterator->value;

        Entity child = 0;

        AddComponent(entity, component);
        switch(propertyKind) {
            case PropertyKind_Value:
                DeserializeValue(entity, property, entityMap, document, reader);
                break;
            case PropertyKind_Child:
                if(entityMap.HasMember(reader)) {
                    GetPropertyValue(property, entity, &child);
                    DeserializeEntity(child, entityMap[reader], entityMap, document);
                }
                break;
            case PropertyKind_Array:
                if(!reader.IsArray()) {
                    Log(entity, LogSeverity_Error, "Property %s is an array property, but JSON value is %s. Skipping this property", GetName(property), GetJsonTypeName(reader));
                    continue;
                }
                auto count = reader.Size();
                SetArrayPropertyCount(property, entity, count);

                for(auto i = 0; i < count; ++i) {
                    auto uuid = reader[i].GetString();

                    if(entityMap.HasMember(uuid)) {
                        DeserializeEntity(GetArrayPropertyElement(property, entity, i), entityMap[uuid], entityMap,
                                          document);
                    }
                }
                break;
        }
    }

    return true;
}

static Entity ResolveEntity(StringRef uuid, rapidjson::Value& entityMap, rapidjson::Document& document) {
    if(entityMap.HasMember(uuid) && !entityMap[uuid].HasMember("$resolved")) {
        // Entity needs to be deserialized
        auto& value = entityMap[uuid];

        auto uuid = Intern(value["Uuid"].GetString());
        auto ownerUuid = Intern(value["$owner"].GetString());
        auto ownerPropertyUuid = Intern(value["$property"].GetString());

        value.AddMember("$resolved", true, document.GetAllocator());

        auto owner = ResolveEntity(ownerUuid, entityMap, document);
        auto ownerProperty = ResolveEntity(ownerPropertyUuid, entityMap, document);

        if(!owner) {
            Log(0, LogSeverity_Error, "Owner is not defined for object '%s'.", uuid);
            return 0;
        }

        if(!ownerProperty) {
            Log(0, LogSeverity_Error, "Owner property is not defined for object '%s'.", uuid);
            return 0;
        }

        Entity entity = FindEntityByUuid(uuid);
        if(!IsEntityValid(entity)) {
            if(GetPropertyKind(ownerProperty) == PropertyKind_Child) {
                GetPropertyValue(ownerProperty, owner, &entity);
                Assert(owner, entity);
            } else {
                auto& indexValue = value["$index"];
                if(indexValue.GetType() != rapidjson::kNumberType) {
                    Log(0, LogSeverity_Error, "Owner property index is not defined for object '%s'.", uuid);
                    return 0;
                }

                auto index = indexValue.GetInt();
                if(GetArrayPropertyCount(ownerProperty, owner) <= index) {
                    SetArrayPropertyCount(ownerProperty, owner, index + 1);
                }

                entity = GetArrayPropertyElement(ownerProperty, owner, index);
                Assert(owner, entity);
            }

            SetUuid(entity, uuid);
        }

        DeserializeEntity(entity, value, entityMap, document);

        return entity;
    }

    // Entity has been deserialized or is external
    auto entity = FindEntityByUuid(uuid);
    if(!IsEntityValid(entity)) {
        entity = FindEntityByName(ComponentOf_Property(), uuid); // Properties, and properties only, can be looked up by name
    }

    return entity;
}

static StringRef ReadNode(rapidjson::Value& value, rapidjson::Value& entityTable, rapidjson::Document& document, StringRef owner, StringRef ownerPropertyName, int index, StringRef existingUuid) {
    if(value.IsString()) {
        return value.GetString();
    }

    if(!value.IsObject()) {
        Log(0, LogSeverity_Error, "ReadNode: Json value is not an object, but a %s. Skipping this node.", GetJsonTypeName(value));
        return "";
    }

    if(!value.HasMember("Uuid")) {
        value.AddMember("Uuid", rapidjson::StringRef(Intern(existingUuid ? existingUuid : CreateGuid())), document.GetAllocator());
    }

    if(!value.HasMember("$owner")) {
        value.AddMember("$owner", rapidjson::StringRef(Intern(owner)), document.GetAllocator());
    }

    if(!value.HasMember("$property")) {
        value.AddMember("$property", rapidjson::StringRef(Intern(ownerPropertyName)), document.GetAllocator());
    }

    if(!value.HasMember("$index")) {
        value.AddMember("$index", index, document.GetAllocator());
    }

    auto uuid = value["Uuid"].GetString();

    for (auto propertyIterator = value.MemberBegin();
         propertyIterator != value.MemberEnd(); ++propertyIterator) {
        auto propertyName = propertyIterator->name.GetString();

        if (propertyName[0] == '$') {
            continue;
        }

        switch (propertyIterator->value.GetType()) {
            case rapidjson::kObjectType:
            {
                auto& element = propertyIterator->value;

                // Avoid treating a vector as an object
                if(!element.HasMember("x")) {
                    auto childUuid = ReadNode(element, entityTable, document, uuid, propertyName, 0, NULL);

                    element.SetString(rapidjson::StringRef(childUuid));
                }
                break;
            }

            case rapidjson::kArrayType:
            {
                for (auto i = 0; i < propertyIterator->value.Size(); ++i) {
                    auto& element = propertyIterator->value[i];
                    if(element.GetType() == rapidjson::kObjectType) {
                        auto childUuid = ReadNode(element, entityTable, document, uuid, propertyName, i, NULL);

                        element.SetString(rapidjson::StringRef(childUuid));
                    }
                }
                break;
            }
        }
    }

    if(value.HasMember("Name") && value.HasMember("PropertyType")) {
        entityTable.AddMember(rapidjson::Value(value["Name"], document.GetAllocator()), rapidjson::Value(value, document.GetAllocator()), document.GetAllocator());
    }

    auto& tableValue = entityTable.AddMember(rapidjson::StringRef(uuid), rapidjson::StringRef(uuid), document.GetAllocator());

    value.Swap(entityTable[uuid]);

    return uuid;
}


static bool DeserializeValue(Entity parent, Entity property, rapidjson::Value& entityMap, rapidjson::Document& document, const rapidjson::Value& reader) {
    auto type = GetPropertyType(property);
    auto jsonType = reader.GetType();
    auto typeName = GetTypeName(type);
    char valueData[64];

    switch(type) {
        ReadIf(u8, Uint)
        ReadIf(u16, Uint)
        ReadIf(u32, Uint)
        ReadIf(u64, Uint64)
        ReadIf(Date, Uint64)
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
                Log(parent, LogSeverity_Error, "Error parsing property '%s': Types should be noted by it's name.", GetName(property));
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
                Log(parent, LogSeverity_Error, "Error parsing property '%s': Entity references should be noted by a path string in JSON. Use $children to note entity hierarchy data.", GetName(property));
                break;
            }

            char absolutePath[PathMax];
            auto id = reader.GetString();

            Entity entity = ResolveEntity(id, entityMap, document);
            if(!IsEntityValid(entity)) {
                entity = FindEntityByPath(id);
            }

            *(Entity*)valueData = entity;
            if(!IsEntityValid(entity)) {
                Log(parent, LogSeverity_Error, "Could not find entity '%s' referenced by property %s.", id, GetName(property));
            }
        }
            break;
        default:
            Log(parent, LogSeverity_Error, "Unsupported type when deserializing property '%s': %s", GetName(property), typeName);
            return false;
            break;
    }

    SetPropertyValue(property, parent, valueData);

    return true;
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

    rapidjson::Value entityMap(rapidjson::kObjectType);
    auto uuid = ReadNode(document, entityMap, document, "", "", 0, GetUuid(entity));
    DeserializeCodeModel(entityMap, document);
    DeserializeEntity(entity, entityMap[uuid], entityMap, document);

    return true;
}

API_EXPORT bool SerializeJson(Entity stream, Entity entity, Entity settings) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    auto result = SerializeNode(
            entity,
            entity,
            "",
            writer,
            GetSerializationSettingsMaxChildLevel(settings),
            GetSerializationSettingsMaxReferenceLevel(settings),
            !GetSerializationSettingsExcludePersistedChildren(settings),
            !GetSerializationSettingsExcludeNativeEntities(settings));

    writer.Flush();

    bool streamWasOpen = IsStreamOpen(stream);
    if(!streamWasOpen) {
        Assert(stream, StreamOpen(stream, StreamMode_Write));
    }

    auto numWritten = StreamWrite(stream, buffer.GetLength(), buffer.GetString());
    if(numWritten != buffer.GetLength()) {
        result = false;
    }

    if(!streamWasOpen) {
        StreamClose(stream);
    }

    return result;
}

API_EXPORT bool DeserializeJson(Entity stream, Entity entity, Entity settings) {
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
    Assert(stream, size == StreamRead(stream, size, data));

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

    rapidjson::Value entityMap(rapidjson::kObjectType);
    auto uuid = ReadNode(document, entityMap, document, "", "", 0, GetUuid(entity));
    entityMap[uuid]["$owner"] = rapidjson::StringRef(GetUuid(GetOwner(entity)));
    entityMap[uuid]["$property"] = rapidjson::StringRef(GetUuid(GetOwnerProperty(entity)));

    /*rapidjson::StringBuffer f;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(f);
    entityMap.Accept(writer);

    printf("%s", f.GetString());*/

    SetUuid(entity, uuid);
    DeserializeCodeModel(entityMap, document);
    DeserializeEntity(entity, entityMap[uuid], entityMap, document);

    free(data);

    return true;
}

static bool Serialize(Entity persistancePoint) {
    return SerializeJson(persistancePoint, persistancePoint, GetPersistancePointSerializationSettings(ModuleOf_Foundation()));
}

static bool Deserialize(Entity persistancePoint) {
    return DeserializeJson(persistancePoint, persistancePoint, GetPersistancePointSerializationSettings(ModuleOf_Foundation()));
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
