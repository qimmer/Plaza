//
// Created by Kim Johannsen on 02/01/2018.
//

#include <Core/Property.h>
#include <Core/String.h>
#include <Core/Pool.h>
#include "Dictionary.h"

struct PropertyData
{
    std::string name;
    void *getter;
    void *setter;
    Type type, owner;
    PropertyTransferFunc transferFunc;
};

DefineHandle(Property, PropertyData)

static Dictionary<String, Property> PropertyTable;

Type GetPropertyType(Property property) {
    return PropertyAt(property)->type;
}

void *GetPropertyGetter(Property property) {
    return PropertyAt(property)->getter;
}

void *GetPropertySetter(Property property) {
    return PropertyAt(property)->setter;
}

const char * GetPropertyName(Property property) {
    return PropertyAt(property)->name.c_str();
}

void SetPropertyType(Property property, Type type) {
    PropertyAt(property)->type = type;
}

void SetPropertyGetter(Property property, void *getter) {
    PropertyAt(property)->getter = getter;
}

void SetPropertySetter(Property property, void *setter) {
    PropertyAt(property)->setter = setter;
}

void SetPropertyName(Property property, const char * name) {
    PropertyAt(property)->name = name;
    PropertyTable[name] = property;
}

Type GetPropertyOwner(Property property) {
    return PropertyAt(property)->owner;
}

void SetPropertyOwner(Property property, Type type) {
    PropertyAt(property)->owner = type;
}

Property FindPropertyByName(StringRef name) {
    auto it = PropertyTable.find(name);
    if(it != PropertyTable.end()) {
        return it->second;
    }
    return 0;
}

PropertyTransferFunc GetPropertyTransferFunc(Property property) {
    return PropertyAt(property)->transferFunc;
}

void SetPropertyTransferFunc(Property property, PropertyTransferFunc func) {
    PropertyAt(property)->transferFunc = func;
}
