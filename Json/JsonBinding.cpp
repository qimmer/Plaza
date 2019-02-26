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

static bool ParseBinding(Entity binding, StringRef sourceBindingString) {
    AddComponent(binding, ComponentOf_Binding());

    auto data = GetBindingData(binding);

    SetNumBindingIndirections(binding, 0);

    auto len = strlen(sourceBindingString);
    auto buffer = (char*)alloca(len + 1);
    strcpy(buffer, sourceBindingString);

    StringRef sourceEntityUuid, propertiesString;
    Split(buffer, '@', &propertiesString, &sourceEntityUuid);

    StringRef propertyName;
    do {
        propertyName = Tokenize((char*)propertiesString, '.');
        auto prototypeNameLen = strlen(propertyName);

        char *propertyUuid = (char*)alloca(prototypeNameLen + 1 + 9);
        sprintf(propertyUuid, "Property.%s", propertyName);
        auto property = FindEntityByUuid(propertyUuid);

        if(!IsEntityValid(property)) {
            Error(binding, "Cannot find property with Uuid '%s'.", propertyUuid);
            return false;
        }

        auto indirection = AddBindingIndirections(binding);
        SetBindingIndirectionProperty(indirection, property);
    }
    while(propertyName > propertiesString);

    if(*sourceEntityUuid) {
        auto reference = AddUnresolvedReferences(binding);
        SetUnresolvedReferenceUuid(reference, sourceEntityUuid);
        SetUnresolvedReferenceProperty(reference, PropertyOf_BindingSourceEntity());
    } else {
        // If no '@', we use self as our source entity
        SetBindingSourceEntity(binding, 0);
    }

    return true;
}

API_EXPORT bool Bind(Entity entity, Entity property, StringRef sourceBindingString) {
    // First, remove eventual existing binding of this property
    for_children(existingBinding, Bindings, entity) {
        auto data = GetBindingData(existingBinding);
        if(data->BindingTargetProperty == property) {
            return ParseBinding(existingBinding, sourceBindingString);
        }
    }

    auto binding = AddBindings(entity);
    SetBindingTargetProperty(binding, property);

    return ParseBinding(binding, sourceBindingString);
}

API_EXPORT StringRef GetBindingString(Entity binding) {
    static std::stringstream ss;
    ss.str(std::string());

    ss << '{';
    u32 numIndirections = 0;
    auto indirections = GetBindingIndirections(binding, &numIndirections);
    for(auto i = 0; i < numIndirections; ++i) {
        auto property = GetBindingIndirectionProperty(indirections[numIndirections - i - 1]);
        ss << (strrchr(GetUuid(property), '.') + 1);

        if(i != (numIndirections - 1)) {
            ss << '.';
        }
    }

    auto sourceEntity = GetBindingSourceEntity(binding);
    if(IsEntityValid(sourceEntity)) {
        ss << '@' << GetUuid(sourceEntity);
    }

    ss << '}';

    auto result = ss.str();
    return Intern(result.c_str());
}