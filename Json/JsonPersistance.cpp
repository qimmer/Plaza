//
// Created by Kim Johannsen on 20/02/2018.
//

#include "JsonPersistance.h"
#include <Core/Types.h>
#include <Foundation/Stream.h>
#include <Json/NativeUtils.h>
#include <Json/JsonModule.h>
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
#include <unordered_map>
#include <string>

#include <Foundation/NativeUtils.h>
#include <Core/Identification.h>
#include <Core/Algorithms.h>
#include <Core/Std.h>
#include <Core/Enum.h>
#include <Core/Binding.h>
#include <Core/Instance.h>

#include "JsonBinding.h"

using namespace eastl;

typedef rapidjson::Writer<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> WriterType;

const StringRef jsonTypeNames[] = {
    "Null",
    "False",
    "True",
    "Object",
    "Array",
    "String",
    "Number"
};

#define ReadIf(TYPE, JSONREADFUNC, READER)\
    case TypeOf_ ## TYPE:\
        if(READER.Is ## JSONREADFUNC ()) {\
            *(TYPE*)&value.data = READER.Get ## JSONREADFUNC ();\
        } else {\
            Log(0, LogSeverity_Error, "Property '%s %s' could not be deserialized: Type is %s but %s was expected.", GetTypeName(GetPropertyType(property)), GetUuid(property), jsonTypeNames[READER.GetType()], #JSONREADFUNC);\
        }\
        break;

#define ReadVec2If(TYPE, JSONREADFUNC, READER)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        if(READER.IsObject()) { \
            v.x = READER.GetObject()["x"].Get ## JSONREADFUNC ();\
            v.y = READER.GetObject()["y"].Get ## JSONREADFUNC ();\
        }\
        if(READER.IsArray()) {\
            v.x = READER.GetArray()[0].Get ## JSONREADFUNC ();\
            v.y = READER.GetArray()[1].Get ## JSONREADFUNC ();\
        }\
        *(TYPE*)&value.data = v;\
        }\
        break;

#define ReadVec3If(TYPE, JSONREADFUNC, READER)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        if(READER.IsObject()) { \
            v.x = READER.GetObject()["x"].Get ## JSONREADFUNC ();\
            v.y = READER.GetObject()["y"].Get ## JSONREADFUNC ();\
            v.z = READER.GetObject()["z"].Get ## JSONREADFUNC ();\
        }\
        if(READER.IsArray()) {\
            v.x = READER.GetArray()[0].Get ## JSONREADFUNC ();\
            v.y = READER.GetArray()[1].Get ## JSONREADFUNC ();\
            v.z = READER.GetArray()[2].Get ## JSONREADFUNC ();\
        }\
        *(TYPE*)&value.data = v;\
        }\
        break;

#define ReadVec4If(TYPE, JSONREADFUNC, READER)\
    case TypeOf_ ## TYPE:\
        {\
        TYPE v;\
        if(READER.IsObject()) { \
            v.x = READER.GetObject()["x"].Get ## JSONREADFUNC ();\
            v.y = READER.GetObject()["y"].Get ## JSONREADFUNC ();\
            v.z = READER.GetObject()["z"].Get ## JSONREADFUNC ();\
            v.w = READER.GetObject()["w"].Get ## JSONREADFUNC ();\
        }\
        if(READER.IsArray()) {\
            v.x = READER.GetArray()[0].Get ## JSONREADFUNC ();\
            v.y = READER.GetArray()[1].Get ## JSONREADFUNC ();\
            v.z = READER.GetArray()[2].Get ## JSONREADFUNC ();\
            v.w = READER.GetArray()[3].Get ## JSONREADFUNC ();\
        }\
        *(TYPE*)&value.data = v;\
        }\
        break;

#define WriteIf(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
        writer.JSONWRITEFUNC(*(TYPE*)&value.data);\
        break;

#define WriteVec2If(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
        {\
            auto v2 = *(TYPE*)&value.data;\
            writer.StartArray();\
            writer.JSONWRITEFUNC(isfinite((double)v2.x) ? v2.x : 0.0);\
            writer.JSONWRITEFUNC(isfinite((double)v2.y) ? v2.y : 0.0);\
            writer.EndArray();\
        }\
        break;

#define WriteVec3If(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
{\
        auto v3 = *(TYPE*)&value.data;\
        writer.StartArray();\
        writer.JSONWRITEFUNC(isfinite((double)v3.x) ? v3.x : 0.0);\
        writer.JSONWRITEFUNC(isfinite((double)v3.y) ? v3.y : 0.0);\
        writer.JSONWRITEFUNC(isfinite((double)v3.z) ? v3.z : 0.0);\
        writer.EndArray();\
        }\
        break;

#define WriteVec4If(TYPE, JSONWRITEFUNC)\
    case TypeOf_ ## TYPE:\
{\
        auto v4 = *(TYPE*)&value.data;\
        writer.StartArray();\
        writer.JSONWRITEFUNC(isfinite((double)v4.x) ? v4.x : 0.0);\
        writer.JSONWRITEFUNC(isfinite((double)v4.y) ? v4.y : 0.0);\
        writer.JSONWRITEFUNC(isfinite((double)v4.z) ? v4.z : 0.0);\
        writer.JSONWRITEFUNC(isfinite((double)v4.w) ? v4.w : 0.0);\
        writer.EndArray();\
        }\
        break;


static bool SerializeNode(JsonSettings *settings, Entity parent, Entity root, WriterType& writer, s16 includeChildLevels, s16 includeReferenceLevels, bool includePersistedChildren, bool includeNativeEntities);
static bool DeserializeValue(Entity stream, Entity parent, Entity property, rapidjson::Document& document, rapidjson::Value& reader);
static Entity ResolveEntity(Entity stream, StringRef uuid, rapidjson::Document& document);

static bool SerializeValue(JsonSettings *settings, Entity entity, Entity property, Entity root, WriterType& writer) {
    Variant value = GetPropertyValue(property, entity);
    auto type = GetPropertyType(property);

    if(type == TypeOf_Variant) {
        writer.StartObject();
        writer.String("type");
        writer.String(GetTypeName(value.type));
        writer.String("value");
    }

    if(type == TypeOf_double && !isfinite((double)value.as_double)) {
        value.as_double = 0.0;
    }

    if(type == TypeOf_float && !isfinite((double)value.as_float)) {
        value.as_float = 0.0f;
    }

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
        WriteIf(float, Double)
        WriteIf(double, Double)
        WriteIf(bool, Bool)
        WriteVec2If(v2f, Double)
        WriteVec3If(v3f, Double)
        WriteVec4If(v4f, Double)
        WriteVec2If(v2i, Int)
        WriteVec3If(v3i, Int)
        WriteVec4If(v4i, Int)
        WriteVec4If(rgba32, Double)
        WriteVec4If(rgba8, Int)
        case TypeOf_StringRef:\
            writer.String(value.as_StringRef ? value.as_StringRef : "");\
            break;
        case TypeOf_NativePtr:
            writer.Uint64((u64)value.as_NativePtr);
            break;
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
            Log(entity, LogSeverity_Warning, "Unsupported type when serializing property '%s': %s", GetUuid(property), GetTypeName(value.type));
            writer.Null();
    }

    if(type == TypeOf_Variant) {
        writer.EndObject();
    }

    return true;
}

static bool SerializeNode(JsonSettings *settings, Entity parent, Entity root, StringRef parentProperty, WriterType& writer, u32 level) {
    bool result = true;

    // Only serialize if max child level has not been reached
    if(!settings || level <= settings->JsonSettingsMaxRecursiveLevels) {
        if(!settings || settings->JsonSettingsExplicitComponents) {
            writer.String("$components");
            writer.StartArray();
            for_entity(component, componentData, Component) {
                if(HasComponent(parent, component)) {
                    writer.String(GetUuid(component));
                }
            }
            writer.EndArray();
        }

        writer.StartObject();

        if(settings->JsonSettingsExplicitHandle) {
            writer.String("$handle");
            writer.Uint64(parent);
        }

        auto& bindingData = GetBindingData(parent);
        if(bindingData.Bindings.size() && settings->JsonSettingsExplicitBindings) {
            writer.String("$bindings");
            writer.StartArray();
            for(auto& binding : bindingData.Bindings) {
                writer.StartObject();

                writer.String("BindingTargetProperty");
                auto uuid = GetUuid(binding.BindingTargetProperty);
                writer.String(uuid ? uuid : "<None>");

                writer.String("BindingSourceEntity");
                uuid = GetUuid(binding.BindingSourceEntity);
                writer.String(uuid ? uuid : "<None>");

                writer.String("BindingIndirections");
                writer.StartArray();
                for(auto& indirection : binding.BindingIndirections) {
                    writer.StartObject();

                    writer.String("IndirectionProperty");
                    uuid = GetUuid(indirection.IndirectionProperty);
                    writer.String(uuid ? uuid : "<None>");

                    writer.String("ListenerEntity");
                    uuid = GetUuid(indirection.ListenerEntity);
                    writer.String(uuid ? uuid : "<None>");

                    writer.EndObject();
                }
                writer.EndArray();

                writer.EndObject();
            }
            writer.EndArray();
        }

        if(bindingData.Listeners.size() && settings->JsonSettingsExplicitBindings) {
            writer.String("$bindingListeners");
            writer.StartArray();
            for(auto& listener : bindingData.Listeners) {
                writer.StartObject();

                writer.String("BindingTargetProperty");
                auto uuid = GetUuid(listener.BindingTargetProperty);
                writer.String(uuid ? uuid : "<None>");

                writer.String("ListenerProperty");
                uuid = GetUuid(listener.ListenerProperty);
                writer.String(uuid ? uuid : "<None>");

                writer.String("BindingEntity");
                uuid = GetUuid(listener.BindingEntity);
                writer.String(uuid ? uuid : "<None>");

                writer.String("BindingIndirectionIndex");
                writer.Uint(listener.BindingIndirectionIndex);

                writer.EndObject();
            }
            writer.EndArray();
        }

        for_entity(component, componentData, Component) {
            if(!HasComponent(parent, component)
               || (parent == root && (component == ComponentOf_PersistancePoint()))
               || component == ComponentOf_Ownership()) {
                continue;
            }



            u32 numProperties = 0;
            auto properties = GetProperties(component, &numProperties);
            for(auto pi = 0; pi < numProperties; ++pi) {
                auto property = properties[pi];

                StringRef uuid = GetUuid(property);
                StringRef name = strrchr(uuid, '.');
                name = name ? (name + 1) : uuid;

                auto kind = GetPropertyKind(property);

                auto binding = GetBinding(parent, property);
                if(binding && kind == PropertyKind_Value && (!settings || !settings->JsonSettingsResolveBindings)) {
                    auto bindingString = GetBindingString(*binding);
                    writer.String(name);
                    writer.String(bindingString);
                    continue;
                }

                if(GetPropertyReadOnly(property) && settings && settings->JsonSettingsIgnoreReadOnly) {
                    continue;
                }

                switch(kind) {
                    case PropertyKind_Value:
                        writer.String(name);

                        result &= SerializeValue(settings, parent, property, root, writer);

                        break;
                    case PropertyKind_Child:
                    {
                        Entity child = GetPropertyValue(property, parent).as_Entity;
                        if(IsEntityValid(child)) {
                            writer.String(name);
                            result &= SerializeNode(settings, child, root, name, writer, level + 1);
                        }
                    }
                        break;
                    case PropertyKind_Array:
                    {
                        writer.String(name);

                        if(binding && (!settings || !settings->JsonSettingsResolveBindings)) {
                            auto bindingString = GetBindingString(*binding);
                            writer.String(bindingString);
                            break;
                        }

                        auto count = GetArrayPropertyCount(property, parent);
                        writer.StartArray();

                        for(auto i = 0; i < count; ++i) {
                            auto child = GetArrayPropertyElement(property, parent, i);
                            if(IsEntityValid(child)) {
                                result &= SerializeNode(settings, child, root, name, writer, level + 1);
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

static bool DeserializeEntity(Entity stream, Entity entity, rapidjson::Value& value, rapidjson::Document& document) {
    auto components = value.FindMember("$components");
    if(components != value.MemberEnd() && components->value.IsArray()) {
        for(auto arrayIt = components->value.Begin(); arrayIt != components->value.End(); ++arrayIt) {
            if(arrayIt->IsString()) {
                auto componentUuid = arrayIt->GetString();

                auto component = FindEntityByUuid(componentUuid);

                if(!IsEntityValid(component)) {
                    Log(0, LogSeverity_Warning, "Unknown component in JSON: %s", componentUuid);
                } else {
                    AddComponent(entity, component);
                }
            }
        }
    }

    auto include = value.FindMember("$include");
    if(include != value.MemberEnd() && include->value.IsString()) {
        auto filePath = include->value.GetString();

        SetStreamPath(entity, filePath);
        SetPersistancePointLoaded(entity, true);
    }

    // Add all required components
    for (auto propertyIterator = value.MemberBegin();
         propertyIterator != value.MemberEnd(); ++propertyIterator) {
        auto propertyUuid = Intern(propertyIterator->name.GetString());

        if (propertyUuid[0] == '$') {
            continue;
        }

        if (!strchr(propertyUuid, '.')) {
            auto newUuid = (char *) alloca(strlen("Property.") + propertyIterator->name.GetStringLength() + 1);
            sprintf(newUuid, "Property.%s", propertyIterator->name.GetString());
            propertyUuid = newUuid;
        }

        auto property = FindEntityByUuid(propertyUuid);

        if (!HasComponent(property, ComponentOf_Property())) {
            Log(0, LogSeverity_Warning, "Unknown property when deserializing '%s': %s", GetDebugName(entity),
                propertyUuid);
            continue;
        }

        if (property == PropertyOf_Owner() || GetPropertyReadOnly(property)) continue;

        auto component = GetOwner(property);
        AddComponent(entity, component);
    }

    // Deserialize property values
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

        auto property = FindEntityByUuid(propertyUuid);

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
                DeserializeValue(stream, entity, property, document, reader);
                break;
            case PropertyKind_Child:
                if(!reader.IsObject()) {
                    Log(0, LogSeverity_Error, "Json value for child property %s is not an object, but a %s. Skipping this node.", GetUuid(property), GetJsonTypeName(reader));
                    break;
                }
                DeserializeEntity(stream, GetPropertyValue(property, entity).as_Entity, reader, document);
                break;
            case PropertyKind_Array:
                if(!reader.IsArray()) {
                    Log(entity, LogSeverity_Error, "Property %s is an array property, but JSON value is %s. Skipping this property", GetUuid(property), GetJsonTypeName(reader));
                    continue;
                }

                auto count = reader.Size();

                for(auto i = 0; i < count; ++i) {
                    auto index = AddArrayPropertyElement(property, entity);
                    Entity child = GetArrayPropertyElement(property, entity, index);
                    DeserializeEntity(stream, child, reader[i], document);
                }
                break;
        }
    }

    return true;
}

static bool ParseBinding(Entity stream, StringRef bindingString, Entity parent, Entity parentProperty, rapidjson::Document& document) {
    auto len = strlen(bindingString);
    if(bindingString[0] != '{' || bindingString[len - 1] != '}') return false;

    char *bindingWithoutCurlyBraces = (char*)alloca(len - 1);
    memcpy(bindingWithoutCurlyBraces, bindingString + 1, len - 2);
    bindingWithoutCurlyBraces[len - 2] = '\0';

    return BindByString(parent, parentProperty, bindingWithoutCurlyBraces);
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

static bool DeserializeValue(Entity stream, Entity parent, Entity property, rapidjson::Document& document, rapidjson::Value& reader) {
    auto type = GetPropertyType(property);
    auto jsonType = reader.GetType();
    auto typeName = GetTypeName(type);

    auto enu = GetPropertyEnum(property);
    if(enu && reader.GetType() == rapidjson::kStringType) {
        auto flagString = reader.GetString();
        u32 cnt = 0;
        auto flags = GetEnumFlags(enu, &cnt);
        u64 value = 0;
        for(auto i = 0; i < cnt; ++i) {
            auto flagName = strrchr(GetUuid(flags[i]), '.') + 1;
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

    if(reader.IsString() && ParseBinding(stream, reader.GetString(), parent, property, document)) {
        return true;
    }

    Variant value;
    rapidjson::Value::MemberIterator variantValueIt;
    auto isVariant = false;
    if(type == TypeOf_Variant) {
        if(reader.IsObject()) {
            isVariant = true;

            if(!reader.GetObject().HasMember("type")) {
                Error(parent, "Error parsing variant %s for %s. Variant object is missing 'type' field.", GetUuid(property), GetUuid(parent));
                return false;
            }

            if(!reader.GetObject().HasMember("value")) {
                Error(parent, "Error parsing variant %s for %s. Variant object is missing 'value' field.", GetUuid(property), GetUuid(parent));
                return false;
            }

            type = FindType(reader.GetObject()["type"].GetString());
            variantValueIt = reader.FindMember("value");
        } else {
            type = GetVariantType(reader);
        }
    }

    value.type = type;

    auto& jsonValue = isVariant ? variantValueIt->value : reader;

    switch(type) {
        ReadIf(u8, Uint, jsonValue)
        ReadIf(u16, Uint, jsonValue)
        ReadIf(u32, Uint, jsonValue)
        ReadIf(u64, Uint64, jsonValue)
        ReadIf(Date, Uint64, jsonValue)
        ReadIf(s8, Int, jsonValue)
        ReadIf(s16, Int, jsonValue)
        ReadIf(s32, Int, jsonValue)
        ReadIf(s64, Int64, jsonValue)
        ReadIf(StringRef, String, jsonValue)
        ReadIf(float, Double, jsonValue)
        ReadIf(double, Double, jsonValue)
        ReadIf(bool, Bool, jsonValue)
        ReadVec2If(v2f, Double, jsonValue)
        ReadVec3If(v3f, Double, jsonValue)
        ReadVec4If(v4f, Double, jsonValue)
        ReadVec4If(rgba32, Double, jsonValue)
        ReadVec2If(v2i, Int, jsonValue)
        ReadVec3If(v3i, Int, jsonValue)
        ReadVec4If(v4i, Int, jsonValue)
        ReadVec4If(rgba8, Int, jsonValue)
        case TypeOf_Type:
            if(!jsonValue.IsString()) {
                Log(parent, LogSeverity_Error, "Error parsing property '%s': Types should be noted by it's type name.", GetUuid(property));
                break;
            }
            value.as_Type = FindType(jsonValue.GetString());
            break;
        case TypeOf_Entity:
        {
            if(jsonValue.IsNull()) {
                value.as_Entity = 0;
                break;
            } else if(!jsonValue.IsString()) {
                Log(parent, LogSeverity_Error, "Error parsing property '%s': Entity references should be noted by Uuid.", GetUuid(property));
                break;
            }

            char absolutePath[PathMax];
            auto id = jsonValue.GetString();

            Entity entity = FindEntityByUuid(id);

            value.as_Entity = entity;
            if(!IsEntityValid(entity)) {
                // If not found, mark it as unresolved and set it when the referenced entity is loaded
                auto unresolvedReference = AddUnresolvedReferences(parent);
                Assert(parent, IsEntityValid(parent));
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

    serializationLevel++;
    DeserializeEntity(stream, entity, document, document);
    serializationLevel--;

    ResolveReferences();

    return true;
}

API_EXPORT bool SerializeJson(Entity stream, Entity entity, Entity jsonSettings) {
    rapidjson::StringBuffer buffer;
    WriterType writer(buffer);

    auto result = SerializeNode(GetJsonSettingsData(jsonSettings), entity, entity, "", writer, 0);

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

    serializationLevel++;
    DeserializeEntity(stream, entity, document, document);
    serializationLevel--;

    free(data);

    ResolveReferences();

    return true;
}

static bool Serialize(Entity persistancePoint) {
    return SerializeJson(persistancePoint, persistancePoint, persistancePoint);
}

static bool Deserialize(Entity persistancePoint) {
    return DeserializeJson(persistancePoint, persistancePoint);
}

BeginUnit(JsonPersistance)
    BeginComponent(JsonSettings)
        RegisterProperty(bool, JsonSettingsResolveBindings)
        RegisterProperty(bool, JsonSettingsIgnoreReadOnly)
        RegisterProperty(u8, JsonSettingsMaxRecursiveLevels)
        RegisterProperty(bool, JsonSettingsExplicitComponents)
        RegisterProperty(bool, JsonSettingsExplicitBindings)
        RegisterProperty(bool, JsonSettingsExplicitHandle)
    EndComponent()

    RegisterSerializer(JsonSerializer, "application/json")

	ModuleData(
		{
			"FileTypes": [
				{ "Uuid": "FileType.Json", "FileTypeExtension": ".json", "FileTypeMimeType" : "application/json" }
			]
		}
	);
EndUnit()
