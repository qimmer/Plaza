//
// Created by Kim Johannsen on 21/12/2017.
//

#ifndef PLAZA_PROPERTY_H
#define PLAZA_PROPERTY_H

#include <Core/Handle.h>
#include <Core/Type.h>

DeclareHandle(Property)

Type GetPropertyOwner(Property property);
Type GetPropertyType(Property property);
void* GetPropertyGetter(Property property);
void* GetPropertySetter(Property property);
const char * GetPropertyName(Property property);

Property FindPropertyByName(StringRef name);

void SetPropertyOwner(Property property, Type type);
void SetPropertyType(Property property, Type type);
void SetPropertyGetter(Property property, void* getter);
void SetPropertySetter(Property property, void *setter);
void SetPropertyName(Property property, const char * name);

#define DefineProperty(PROPERTYTYPE, PROPERTYNAME) \
        Property prop_ ## PROPERTYNAME = CreateProperty(); \
        SetPropertyOwner(prop_ ## PROPERTYNAME, type); \
        SetPropertyType(prop_ ## PROPERTYNAME, TypeOf_ ## PROPERTYTYPE ()); \
        SetPropertyName(prop_ ## PROPERTYNAME, #PROPERTYNAME ); \
        SetPropertyGetter(prop_ ## PROPERTYNAME, (void*)& Get ## PROPERTYNAME); \
        SetPropertySetter(prop_ ## PROPERTYNAME, (void*)& Set ## PROPERTYNAME);

#endif //PLAZA_PROPERTY_H
