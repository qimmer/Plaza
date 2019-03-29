//
// Created by Kim on 25-02-2019.
//

#include "JsonBinding.h"
#include <Core/Binding.h>
#include <Core/Instance.h>
#include <Core/Identification.h>

#include <sstream>

static StringRef Tokenize(char* string, char delimeter) {
    auto delimeterLocation = strrchr(string, delimeter);
    if(!delimeterLocation) return string;

    *delimeterLocation = '\0';

    return delimeterLocation + 1;
}

static void Split(char* string, char delimeter, StringRef* left, StringRef* right) {
    auto delimeterLocation = strrchr(string, delimeter);
    if(delimeterLocation) {
        *delimeterLocation = '\0';
    }

    *left = string;
    *right = delimeterLocation ? (delimeterLocation + 1) : "";
}

static bool ParseBinding(Entity entity, Entity targetProperty, StringRef sourceBindingString) {
    Vector<Entity, 16> indirections;
    Vector<StringRef, 16> indirectionArrayNames;

    auto len = strlen(sourceBindingString);
    auto buffer = (char*)alloca(len + 1);
    strcpy(buffer, sourceBindingString);

    StringRef sourceEntityUuid, propertiesString;
    Split(buffer, '@', &propertiesString, &sourceEntityUuid);

    StringRef propertyName;
    do {
        propertyName = Tokenize((char*)propertiesString, '.');

        auto nameStart = strchr(propertyName, '[');
        auto nameEnd = strchr(propertyName, ']');
        if(nameStart && nameEnd) {
            *nameStart = '\0';
            *nameEnd = '\0';
            auto name = Intern(nameStart + 1);
            indirectionArrayNames.push_back(name);
        } else {
            indirectionArrayNames.push_back(0);
        }

        auto prototypeNameLen = strlen(propertyName);

        char *propertyUuid = (char*)alloca(prototypeNameLen + 1 + 9);
        sprintf(propertyUuid, "Property.%s", propertyName);
        auto property = FindEntityByUuid(propertyUuid);

        if(!IsEntityValid(property)) {
            Error(0, "Cannot find property with Uuid '%s'.", propertyUuid);
            return false;
        }

        indirections.push_back(property);
    }
    while(propertyName > propertiesString);

    if(*sourceEntityUuid) {
        auto sourceEntity = FindEntityByUuid(sourceEntityUuid);
        if(!sourceEntity) {
            Error(0, "Cannot find source entity with Uuid '%s'.", sourceEntityUuid);
            return false;
        }

        Bind(entity, targetProperty, sourceEntity, indirections.data(), indirectionArrayNames.data(), indirections.size());
    } else {
        // If no '@', we use self as our source entity
        Bind(entity, targetProperty, 0, indirections.data(), indirectionArrayNames.data(), indirections.size());
    }

    return true;
}

API_EXPORT bool BindByString(Entity entity, Entity property, StringRef sourceBindingString) {
    return ParseBinding(entity, property, sourceBindingString);
}

API_EXPORT StringRef GetBindingString(const Binding& binding) {
    static std::stringstream ss;
    ss.str(std::string());

    ss << '{';

    auto numIndirections = binding.BindingIndirections.size();
    for(auto i = 0; i < numIndirections; ++i) {
        auto property = binding.BindingIndirections[numIndirections - i - 1].IndirectionProperty;
        ss << (strrchr(GetIdentification(property).Uuid, '.') + 1);

        if(i != (numIndirections - 1)) {
            ss << '.';
        }
    }

    if(IsEntityValid(binding.BindingSourceEntity)) {
        ss << '@' << GetUuid(binding.BindingSourceEntity);
    }

    ss << '}';

    auto result = ss.str();
    return Intern(result.c_str());
}