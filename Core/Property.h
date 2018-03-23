//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_PROPERTY_H
#define PLAZA_PROPERTY_H

#include <Core/Handle.h>
#include <Core/Type.h>
#include <Core/Enum.h>

DeclareHandle(Property)

typedef void(*PropertyTransferFunc)(Handle src, Handle dst);

Type GetPropertyOwner(Property property);
Type GetPropertyType(Property property);
void* GetPropertyGetter(Property property);
void* GetPropertySetter(Property property);
const char * GetPropertyName(Property property);
PropertyTransferFunc GetPropertyTransferFunc(Property property);
Enum GetPropertyEnum(Property property);

Property FindPropertyByName(StringRef name);

void SetPropertyOwner(Property property, Type type);
void SetPropertyType(Property property, Type type);
void SetPropertyGetter(Property property, void* getter);
void SetPropertySetter(Property property, void *setter);
void SetPropertyName(Property property, const char * name);
void SetPropertyTransferFunc(Property property, PropertyTransferFunc func);
void SetPropertyEnum(Property property, Enum e);

#define DefineProperty(PROPERTYTYPE, PROPERTYNAME) \
        Property prop_ ## PROPERTYNAME = CreateProperty(); \
        SetPropertyOwner(prop_ ## PROPERTYNAME, type); \
        SetPropertyType(prop_ ## PROPERTYNAME, TypeOf_ ## PROPERTYTYPE ()); \
        SetPropertyName(prop_ ## PROPERTYNAME, #PROPERTYNAME ); \
        SetPropertyGetter(prop_ ## PROPERTYNAME, (void*)& Get ## PROPERTYNAME); \
        SetPropertySetter(prop_ ## PROPERTYNAME, (void*)& Set ## PROPERTYNAME); \
        SetPropertyTransferFunc(prop_ ## PROPERTYNAME, Copy ## PROPERTYNAME);

#define DefinePropertyEnum(PROPERTYTYPE, PROPERTYNAME, ENUM) \
        DefineProperty(PROPERTYTYPE, PROPERTYNAME)\
        SetPropertyEnum(prop_ ## PROPERTYNAME, EnumOf_ ## ENUM ());

#endif //PLAZA_PROPERTY_H
