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
#include <Core/Algorithms.h>

#include <unordered_map>
#include <string>
#include <Core/Std.h>
#include <Core/Enum.h>
#include <Core/Binding.h>
#include <Core/Instance.h>

using namespace eastl;

const StringRef jsonTypeNames[] = {
    "Null",
    "False",
    "True",
    "Object",
    "Array",
    "String",
    "Number"
};

#define WriteIf(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
        writer.JSONWRITEFUNC(*(TYPE*)&value.data);\
        break;

#define ReadIf(TYPE, JSONREADFUNC)\
    case TypeOf_ ## TYPE:\
        if(reader.Is ## JSONREADFUNC ()) {\
            *(TYPE*)&value.data = reader.Get ## JSONREADFUNC ();\
        } else {\
            Log(0, LogSeverity_Error, "Property '%s %s' could not be deserialized: Type is %s but %s was expected.", GetTypeName(GetPropertyType(property)), GetName(property), jsonTypeNames[reader.GetType()], #JSONREADFUNC);\
        }\
        break;

#define ReadVec2If(TYPE, JSONREADFUNC)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        if(reader.IsObject()) { \
            v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
            v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
        }\
        if(reader.IsArray()) {\
            v.x = reader.GetArray()[0].Get ## JSONREADFUNC ();\
            v.y = reader.GetArray()[1].Get ## JSONREADFUNC ();\
        }\
        *(TYPE*)&value.data = v;\
        }\
        break;

#define ReadVec3If(TYPE, JSONREADFUNC)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        if(reader.IsObject()) { \
            v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
            v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
            v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
        }\
        if(reader.IsArray()) {\
            v.x = reader.GetArray()[0].Get ## JSONREADFUNC ();\
            v.y = reader.GetArray()[1].Get ## JSONREADFUNC ();\
            v.z = reader.GetArray()[2].Get ## JSONREADFUNC ();\
        }\
        *(TYPE*)&value.data = v;\
        }\
        break;

#define ReadVec4If(TYPE, JSONREADFUNC)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        if(reader.IsObject()) { \
            v.x = reader.GetObject()["x"].Get ## JSONREADFUNC ();\
            v.y = reader.GetObject()["y"].Get ## JSONREADFUNC ();\
            v.z = reader.GetObject()["z"].Get ## JSONREADFUNC ();\
            v.w = reader.GetObject()["w"].Get ## JSONREADFUNC ();\
        }\
        if(reader.IsArray()) {\
            v.x = reader.GetArray()[0].Get ## JSONREADFUNC ();\
            v.y = reader.GetArray()[1].Get ## JSONREADFUNC ();\
            v.z = reader.GetArray()[2].Get ## JSONREADFUNC ();\
            v.w = reader.GetArray()[3].Get ## JSONREADFUNC ();\
        }\
        *(TYPE*)&value.data = v;\
        }\
        break;

#define WriteVec2If(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
        {\
            auto v2 = *(TYPE*)&value.data;\
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
        auto v3 = *(TYPE*)&value.data;\
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
        auto v4 = *(TYPE*)&value.data;\
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
    Variant value = GetPropertyValue(property, entity);

    switch(value.type) {
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
            writer.String(GetTypeName(value.as_Type));
            break;
        case TypeOf_Entity:
        {
            auto entity = value.as_Entity;
            if(IsEntityValid(entity)) {
                writer.String(GetUuid(entity));
            } else {
                writer.Null();
            }
        }

            break;
        default:
            Log(entity, LogSeverity_Warning, "Unsupported type when serializing property '%s': %s", GetName(property), GetTypeName(value.type));
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
            for_entity(component, componentData, Component, {
                if(HasComponent(parent, component)) {
                    writer.String(GetUuid(component));
                }
            });
            writer.EndArray();
        }

        writer.String("$owner");
        writer.String(GetUuid(GetOwner(parent)));

        for_entity(component, componentData, Component, {
            if(!HasComponent(parent, component)
               || (parent == root && (component == ComponentOf_PersistancePoint()))
               || component == ComponentOf_Ownership()) {
                continue;
            }



            u32 numProperties = 0;
            auto properties = GetProperties(component, &numProperties);
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
                            Entity child = GetPropertyValue(property, parent).as_Entity;

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
                        Entity child = GetPropertyValue(property, parent).as_Entity;
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
        });

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

static bool DeserializeEntity(Entity entity, rapidjson::Value& value, rapidjson::Value& entityMap, rapidjson::Document& document) {
    if(!value.IsObject()) {
        Log(0, LogSeverity_Error, "Json value is not an object, but a %s. Skipping this node.", GetJsonTypeName(value));
        return false;
    }

    value.AddMember("$resolved", entity, document.GetAllocator());

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

    auto instanceTemplate = value.FindMember("InstanceTemplate");
    if(instanceTemplate != value.MemberEnd()) {
        // Make sure to add instance component early on,
        // which tracks what properties are being set and should override
        // template.
        AddComponent(entity, ComponentOf_Instance());
    }

    for (auto propertyIterator = value.MemberBegin();
         propertyIterator != value.MemberEnd(); ++propertyIterator)
    {
        auto propertyUuid = Intern(propertyIterator->name.GetString());

        if (propertyUuid[0] == '$') {
            continue;
        }

        if(!strchr(propertyUuid, '.')) {
            auto newUuid = (char*)alloca(strlen("Property.") + propertyIterator->name.GetStringLength() + 1);
            sprintf(newUuid, "Property.%s", propertyIterator->name.GetString());
            propertyUuid = newUuid;
        }

        auto property = ResolveEntity(propertyUuid, entityMap, document);

        if(!HasComponent(property, ComponentOf_Property())) {
            Log(0, LogSeverity_Warning, "Unknown property when deserializing '%s': %s", GetDebugName(entity), propertyUuid);
            continue;
        }

        if(property == PropertyOf_Owner() || GetPropertyReadOnly(property)) continue;

        auto component = GetOwner(property);

        auto propertyKind = GetPropertyKind(property);
        auto& reader = propertyIterator->value;

        AddComponent(entity, component);
        switch(propertyKind) {
            case PropertyKind_Value:
                DeserializeValue(entity, property, entityMap, document, reader);
                break;
            case PropertyKind_Child:
                if(entityMap.HasMember(reader)) {
                    DeserializeEntity(GetPropertyValue(property, entity).as_Entity, entityMap[reader], entityMap, document);
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
                        Entity child = GetArrayPropertyElement(property, entity, i);
                        DeserializeEntity(child, entityMap[uuid], entityMap,
                                          document);
                    }
                }
                break;
        }
    }

    return true;
}

static Entity ResolveEntity(StringRef uuid, rapidjson::Value& entityMap, rapidjson::Document& document) {
    if(entityMap.HasMember(uuid)) {
        auto& value = entityMap[uuid];

        auto resolvedIt = value.FindMember("$resolved");
        if(resolvedIt != value.MemberEnd()) {
            return (Entity)resolvedIt->value.GetUint64();
        }

        // Entity needs to be deserialized
        auto uuid = Intern(value["Uuid"].GetString());
        auto ownerUuid = Intern(value["$owner"].GetString());
        auto ownerPropertyUuid = Intern(value["$property"].GetString());

        value.AddMember("$resolved", (u64)0, document.GetAllocator());

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
                Assert(owner, GetPropertyValue(ownerProperty, owner).as_Entity);
            } else if(GetPropertyKind(ownerProperty) == PropertyKind_Array) {
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
            } else {
                Assert(owner, false); // Never reach this!
            }

            SetUuid(entity, uuid);
        }

        DeserializeEntity(entity, value, entityMap, document);

        return entity;
    }

    // Entity has been deserialized or is external
    auto entity = FindEntityByUuid(uuid);
    return entity;
}

static StringRef ReadNode(rapidjson::Value& value, rapidjson::Value& entityTable, rapidjson::Document& document, StringRef owner, StringRef ownerPropertyUuid, int index, StringRef existingUuid) {
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
        value.AddMember("$property", rapidjson::StringRef(Intern(ownerPropertyUuid)), document.GetAllocator());
    }

    if(!value.HasMember("$index")) {
        value.AddMember("$index", index, document.GetAllocator());
    }

    auto uuid = value["Uuid"].GetString();

    for (auto propertyIterator = value.MemberBegin();
         propertyIterator != value.MemberEnd(); ++propertyIterator) {
        auto propertyUuid = propertyIterator->name.GetString();

        if (propertyUuid[0] == '$') {
            continue;
        }

        if(!strchr(propertyUuid, '.')) {
            auto newUuid = (char*)alloca(strlen("Property.") + propertyIterator->name.GetStringLength() + 1);
            sprintf(newUuid, "Property.%s", propertyIterator->name.GetString());
            propertyUuid = newUuid;
        }

        switch (propertyIterator->value.GetType()) {
            case rapidjson::kObjectType:
            {
                auto& element = propertyIterator->value;

                if(!element.HasMember("x")) {
                    auto childUuid = ReadNode(element, entityTable, document, uuid, propertyUuid, 0, NULL);

                    element.SetString(rapidjson::StringRef(childUuid));
                }
                break;
            }

            case rapidjson::kArrayType:
            {
                for (auto i = 0; i < propertyIterator->value.Size(); ++i) {
                    auto& element = propertyIterator->value[i];
                    if(element.GetType() == rapidjson::kObjectType) {
                        auto childUuid = ReadNode(element, entityTable, document, uuid, propertyUuid, i, NULL);

                        element.SetString(rapidjson::StringRef(childUuid));
                    }
                }
                break;
            }
        }
    }

    auto& tableValue = entityTable.AddMember(rapidjson::StringRef(uuid), rapidjson::StringRef(uuid), document.GetAllocator());

    value.Swap(entityTable[uuid]);

    return uuid;
}

static bool ParseBinding(StringRef bindingString, Entity parent, Entity parentProperty, rapidjson::Value& entityMap, rapidjson::Document& document) {
    auto len = strlen(bindingString);
    if(bindingString[0] != '{' || bindingString[len - 1] != '}') return false;

    char *bindingWithoutCurlyBraces = (char*)alloca(len - 1);
    memcpy(bindingWithoutCurlyBraces, bindingString + 1, len - 2);
    bindingWithoutCurlyBraces[len - 2] = '\0';

    auto entityName = strchr(bindingWithoutCurlyBraces, '@') + 1;
    auto entity = ResolveEntity(entityName, entityMap, document);

    if(!IsEntityValid(entity)) {
        Log(0, LogSeverity_Error, "Could not find bindable entity: %s", entityName);
        return true;
    }

    Bind(parent, parentProperty, bindingWithoutCurlyBraces);

    return true;
}

static Type GetVariantType(const rapidjson::Value& value) {
    switch(value.GetType()) {
        case rapidjson::kStringType:
            return TypeOf_StringRef;

        case rapidjson::kNumberType:
            if (value.IsInt()) {
                return TypeOf_s32;
            }
            return TypeOf_double;

        case rapidjson::kFalseType:
        case rapidjson::kTrueType:
            return TypeOf_bool;

        case rapidjson::kArrayType:
            if (value.GetArray().Size() < 2 || value.GetArray().Size() > 4) {
                return TypeOf_unknown;
            }
            auto elementType = GetVariantType(value.GetArray()[0]);
            switch (value.GetArray().Size()) {
                case 2:
                    switch (elementType) {
                        case TypeOf_s32:
                            return TypeOf_v2i;
                        case TypeOf_double:
                            return TypeOf_v2f;
                    }
                    break;
                case 3:
                    switch (elementType) {
                        case TypeOf_s32:
                            return TypeOf_v3i;
                        case TypeOf_double:
                            return TypeOf_v3f;
                    }
                    break;
                case 4:
                    switch (elementType) {
                        case TypeOf_s32:
                            return TypeOf_v4i;
                        case TypeOf_double:
                            return TypeOf_v4f;
                    }
                    break;
            }

    }

    Log(0, LogSeverity_Error, "Complex JSON type %s cannot be cast to variant.", jsonTypeNames[value.GetType()]);
    return TypeOf_unknown;
}

static bool DeserializeValue(Entity parent, Entity property, rapidjson::Value& entityMap, rapidjson::Document& document, const rapidjson::Value& reader) {
    auto type = GetPropertyType(property);
    auto jsonType = reader.GetType();
    auto typeName = GetTypeName(type);

    auto enu = GetPropertyEnum(property);
    auto hasEnum = HasComponent(enu, ComponentOf_Enum());
    if(hasEnum && reader.GetType() == rapidjson::kStringType) {
        auto flagString = Intern(reader.GetString());
        u32 cnt = 0;
        auto flags = GetEnumFlags(enu, &cnt);
        u64 value = 0;
        for(auto i = 0; i < cnt; ++i) {
            auto flagName = GetName(flags[i]);
            auto foundStr = strstr(flagString, flagName);
            if(foundStr) {
                auto endChar = *(foundStr + strlen(flagName));
                if(endChar == '\0' || endChar == '|') {
                    value |= GetEnumFlagValue(flags[i]);
                }
            }
        }
        SetPropertyValue(property, parent, __MakeVariant(&value, type));
        return true;
    }

    if(reader.IsString() && ParseBinding(reader.GetString(), parent, property, entityMap, document)) {
        return true;
    }

    Variant value;
    auto isVariant = false;
    if(type == TypeOf_Variant) {
        isVariant = true;

        type = GetVariantType(reader);
    }

    value.type = type;

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
                Log(parent, LogSeverity_Error, "Error parsing property '%s': Types should be noted by it's type name.", GetUuid(property));
                break;
            }
            value.as_Type = FindType(reader.GetString());
            break;
        case TypeOf_Entity:
        {
            if(reader.IsNull()) {
                value.as_Entity = 0;
                break;
            } else if(!reader.IsString()) {
                Log(parent, LogSeverity_Error, "Error parsing property '%s': Entity references should be noted by Uuid.", GetUuid(property));
                break;
            }

            char absolutePath[PathMax];
            auto id = reader.GetString();

            Entity entity = ResolveEntity(id, entityMap, document);
            if(!IsEntityValid(entity) && id[0] == '/') {
                entity = FindEntityByPath(id);
            }

            value.as_Entity = entity;
            if(!IsEntityValid(entity)) {
                // If not found, mark it as unresolved and set it when the referenced entity is loaded
                auto unresolvedReference = AddUnresolvedReferences(parent);
                SetUnresolvedReferenceProperty(unresolvedReference, property);
                SetUnresolvedReferenceUuid(unresolvedReference, id);
            }
        }
            break;
        default:
            Log(parent, LogSeverity_Error, "Unsupported type when deserializing property '%s': %s", GetUuid(property), typeName);
            return false;
            break;
    }

    SetPropertyValue(property, parent, value);

    return true;
}

API_EXPORT bool DeserializeJsonFromString(Entity stream, Entity entity, StringRef jsonString) {
    static s32 serializationLevel = 0;

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

    serializationLevel++;
    DeserializeEntity(entity, entityMap[uuid], entityMap, document);
    serializationLevel--;

    // Only resolve references at root serialization level (after all eventual child serializations have finished
    if(serializationLevel == 0 && !ResolveReferences()) {
        for_entity(unresolvedReference, data, UnresolvedReference, {
            Log(entity, LogSeverity_Error, "%s of entity %s has an unresolved reference: %s",
                    GetUuid(GetUnresolvedReferenceProperty(unresolvedReference)),
                    GetDebugName(GetOwner(unresolvedReference)),
                    GetUnresolvedReferenceUuid(unresolvedReference));
        });
    }

    return true;
}

API_EXPORT bool SerializeJson(Entity stream, Entity entity, s16 includeChildLevels, s16 includeReferenceLevels, bool includePersistedChildren, bool includeNativeEntities) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    auto result = SerializeNode(entity, entity, "", writer, includeChildLevels, includeReferenceLevels, includePersistedChildren, includeNativeEntities);

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

API_EXPORT bool DeserializeJson(Entity stream, Entity entity) {
    static s32 serializationLevel = 0;

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

    SetUuid(entity, uuid);

    serializationLevel++;
    DeserializeEntity(entity, entityMap[uuid], entityMap, document);
    serializationLevel--;

    free(data);

    // Only resolve references at root serialization level (after all eventual child serializations have finished
    if(serializationLevel == 0 && !ResolveReferences()) {
        for_entity(unresolvedReference, data, UnresolvedReference, {
            Log(entity, LogSeverity_Error, "%s of entity %s has an unresolved reference: %s",
                GetName(GetUnresolvedReferenceProperty(unresolvedReference)),
                GetDebugName(GetOwner(unresolvedReference)),
                GetUnresolvedReferenceUuid(unresolvedReference));
        });
    }

    return true;
}

static bool Serialize(Entity persistancePoint) {
    return SerializeJson(persistancePoint, persistancePoint, INT16_MAX, 0, false, false);
}

static bool Deserialize(Entity persistancePoint) {
    return DeserializeJson(persistancePoint, persistancePoint);
}

BeginUnit(JsonPersistance)
    RegisterSerializer(JsonSerializer, "application/json")

	ModuleData(
		{
			"FileTypes": [
				{ "Uuid": "FileType.Json", "FileTypeExtension": ".json", "FileTypeMimeType" : "application/json" }
			]
		}
	);
EndUnit()
